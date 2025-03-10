#include "../../include/Peers/provider.h"
#include "../../include/Peers/bootstrap_node.h"
#include "../../include/RequestResponse/acknowledgement.h"
#include "../../include/RequestResponse/message.h"
#include "../../include/RequestResponse/registration.h"
#include "../../include/RequestResponse/task_request.h"
#include "../../include/utility.h"
#include "proto/payload.pb.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace std;

Provider::Provider(const char* port, string uuid)
    : Peer(uuid), ml_zmq_sender(), ml_zmq_receiver(), aggregator_zmq_sender(),
      aggregator_zmq_receiver() {
    isBusy = false;
    isLocalBootstrap = false;

    cout << "ML ZMQ: Sender: " << ml_zmq_sender.getAddress()
         << ", Receiver: " << ml_zmq_receiver.getAddress() << endl;
    cout << "Aggregator ZMQ: Sender: " << aggregator_zmq_sender.getAddress()
         << ", Receiver: " << aggregator_zmq_receiver.getAddress() << endl;

    setupServer("127.0.0.1", port);
}
Provider::~Provider() noexcept {}

void Provider::registerWithBootstrap() {
    const char* bootstrapHost = BootstrapNode::getServerIpAddress();
    const char* bootstrapPort = BootstrapNode::getServerPort();
    cout << "Connecting to bootstrap node at " << bootstrapHost << ":"
         << bootstrapPort << endl;
    if (client->setupConn(bootstrapHost, bootstrapPort, "tcp") == -1) {
        cerr << "Unable to connect to boostrap node" << endl;
        exit(1);
    }

    shared_ptr<Registration> payload = make_shared<Registration>();
    Message msg(uuid, IpAddress(host, port), payload);

    client->sendMsg(msg.serialize(), -1);
}

void Provider::listen() {
    while (true) {
        cout << "Waiting for requester to connect..." << endl;
        if (!server->acceptConn()) {
            continue;
        }

        // receive task request object from client
        string requesterData;
        if(server->receiveFromConn(requesterData) == 1) {
            server->closeConn();
            continue;
        }

        Message requesterMsg;
        requesterMsg.deserialize(requesterData);
        if (requesterMsg.getPayload()->getType() != Payload::Type::TASK_REQUEST) {
            server->closeConn();
            continue;
        }

        string requesterUuid = requesterMsg.getSenderUuid();
        IpAddress requesterIpAddr = requesterMsg.getSenderIpAddr();
        shared_ptr<Payload> requesterPayload = requesterMsg.getPayload();

        // Parse and assign member field with task request received
        shared_ptr<TaskRequest> taskReq =
            static_pointer_cast<TaskRequest>(requesterPayload);
        taskRequest = make_unique<TaskRequest>(std::move(*taskReq));

        // Ensure task request contains a non-empty training data index field
        if (taskRequest->taskRequestType != TaskRequest::INDEX_FILENAME ||
            taskRequest->getTrainingDataIndexFilename().empty()) {
            server->replyToConn("Provider ID: " + uuid +
                                " : Task request does not contain a valid "
                                "training data index file.");
            server->closeConn();
            continue;
        }

        // Download and parse training data index file from requester
        cout << "FTP: requesting index: "
             << taskRequest->getTrainingDataIndexFilename() << endl;

        // bug here where we are saving the file to the same file
        // server->getFileFTP(taskRequest->getTrainingDataIndexFilename());
        string indexFile = ingestTrainingData();
        server->closeConn();

        // initialize ML.py with metadata
        initializeWorkloadToML();

        if (taskRequest->getLeaderUuid() == uuid) {
            leaderHandleTaskRequest(requesterIpAddr);
        } else {
            followerHandleTaskRequest();
        }
    }
}

void Provider::leaderHandleTaskRequest(const IpAddress& requesterIpAddr) {
    TaskResponse aggregatedResults = TaskResponse();

    for (int i = 0; i < taskRequest->getNumEpochs(); i++) {
        vector<string> followerData{};
        while (followerData.size() < taskRequest->getAssignedWorkers().size() - 1) {
            cout << "\nWaiting for follower peer to connect..." << endl;
            while (!server->acceptConn());

            // get data from followers and aggregate
            string followerMsgStr;
            if (server->receiveFromConn(followerMsgStr) == 1) {
                cerr << "Failed to receive data from follower" << endl;
                server->closeConn();
                continue;
            }

            Message followerMsg;
            followerMsg.deserialize(followerMsgStr);
            shared_ptr<Payload> followerPayload = followerMsg.getPayload();

            if (followerPayload->getType() != Payload::Type::TASK_RESPONSE) {
                server->closeConn();
                continue;
            }

            shared_ptr<TaskResponse> taskResp =
                static_pointer_cast<TaskResponse>(followerPayload);

            // append to followerData
            followerData.push_back(taskResp->getTrainingData());
            server->replyToConn("Received follower result.");
            server->closeConn();
        }

        cout << endl;

        // Aggregate model parameters and send to aggregator script
        aggregatedResults = aggregateResults(followerData);
        

        // Send/Process aggregated results to follower peers
        cout << "Sending aggregated results to followers..." << endl;
        for (const auto& follower : taskRequest->getAssignedWorkers()) {
            if (follower.first == uuid) {
                // if the follower is the leader, skip sending

                // TODO: send the aggregated state dict to ML.py 
                // Create payload containing a ModelStateDictParams message
                
                processWorkload();
                continue;
            }

            IpAddress followerIp = follower.second;
            while (client->setupConn(followerIp, "tcp") != 0) {
                cout << "Failed to connect to follower to send aggregated results, trying again in 5s" << endl;
                sleep(5);
            }

            shared_ptr<TaskResponse> payload = make_shared<TaskResponse>(aggregatedResults);
            Message msg(uuid, IpAddress(host, port), payload);
            int code = client->sendMsg(msg.serialize(), -1);
            cout << "Leader sent data to follower with code " << code << endl;
        }
    }

    // Send results back to requester
    // TODO: requester IP address could change
    shared_ptr<TaskResponse> aggregatePayload =
        make_shared<TaskResponse>(aggregatedResults);
    Message aggregateResultMsg(uuid, IpAddress(host, port), aggregatePayload);

    // busy wait until connection is established
    while (client->setupConn(requesterIpAddr, "tcp") != 0) {
        constexpr int retry = 5;
        cout << "Failed to connect to requester server, trying again in " << retry << "s" << endl;
        sleep(retry);
    }
    cout << "Connected to requester server" << endl;

    if (client->sendMsg(aggregateResultMsg.serialize(), 5) == 1) {
        cerr << "Failed to send aggregated result to requester" << endl;
        return;
    }

    cout << "Sent results to requester. Waiting for acknowledgement" << endl;
    while (!server->acceptConn());

    while (true) {
        // receive response from requester
        string serializedData;
        server->receiveFromConn(serializedData, -1);

        Message msg;
        msg.deserialize(serializedData);
        if (msg.getPayload()->getType() == Payload::Type::ACKNOWLEDGEMENT) {
            cout << "Acknowledgement received from requester" << endl;
            break;
        }
    }

    server->closeConn();
}

void Provider::followerHandleTaskRequest() {

    for (int i = 0; i < taskRequest->getNumEpochs(); i++) {
        // run one aggregation cycle of training
        cout << "Waiting for connection back to leader" << endl;
        IpAddress leaderIp =
            taskRequest->getAssignedWorkers()[taskRequest->getLeaderUuid()];
        // busy wait until connection is established with the leader
        while (client->setupConn(leaderIp, "tcp") == -1) {
            sleep(5);
        }

        // send results back to leader
        shared_ptr<TaskResponse> payload = std::move(taskResponse);
        Message msg(uuid, IpAddress(host, port), payload);
        int code = client->sendMsg(msg.serialize(), -1);
        cout << "Follower sent data to leader with code " << code << endl;

        if (i != taskRequest.getNumEpochs() - 1) {
            // wait for leader to send model state dict param
            while (!server->acceptConn());
            // receive 

            // forward model state dict param to ml 
            processWorkload();
        }
    }
}

void Provider::processWorkload() {

}

string Provider::ingestTrainingData() {
    string trainingDataIndexFile = taskRequest->getTrainingDataIndexFilename();
    vector<string> requiredTrainingFiles = taskRequest->getTrainingDataFiles();
    cout << "Task requires " << requiredTrainingFiles.size()
         << " training files" << endl;
    // Ensure all required training data files are present
    for (const string& filename : requiredTrainingFiles) {
        if (!isFileWithinDataDirectory(filename)) {
            cout << "FTP: requesting " << filename << endl;
            server->getFileFTP(filename);
        }
    }
    cout << "All training files are now present" << endl;

    // Temporarily just send index file to python worker
    return trainingDataIndexFile;
}

void Provider::initializeWorkloadToML() {
    // send training data index file to the worker
    cout << "Working on training data index file: " << indexFile << endl;

    cout << "Sending training data index file to worker..." << endl;
    payload::TrainingData training_data_proto;
    training_data_proto.set_training_data_index_filename(indexFile);
    training_data_proto.set_numepochs(taskRequest->getNumEpochs());
    string serialized_training_data;
    training_data_proto.SerializeToString(&serialized_training_data);
    ml_zmq_sender.send(serialized_training_data);

    cout << "Waiting for processed data..." << endl;
    auto rcvdData = ml_zmq_receiver.receive();
    cout << "Received processed data" << endl;

    // Parse received task response proto and populate TaskResponse object
    payload::TaskResponse task_response_proto;
    task_response_proto.ParseFromString(rcvdData);
    taskResponse = make_unique<TaskResponse>(task_response_proto.modelstatedict());
    cout << "Completed assigned workload" << endl;
}

TaskResponse Provider::aggregateResults(vector<string> followerData) {
    cout << "Aggregating results..." << endl;

    // append leader's data to followerData
    string curr_data = taskResponse->getTrainingData();
    followerData.push_back(curr_data);

    // send each follower's data to the aggregator
    cout << "Sending data to aggregator..." << endl;
    for (int i = 0; i < followerData.size(); i++) {
        cout << "Aggregator for loop: " << i << endl;
        payload::ModelStateDictParams* proto =
            new payload::ModelStateDictParams();
        proto->set_modelstatedict(followerData[i]);
        string serialized;
        proto->SerializeToString(&serialized);
        aggregator_zmq_sender.send(serialized);
    }

    // receive aggregated data from the aggregator
    cout << "Waiting for aggregated data..." << endl;
    auto rcvdData = aggregator_zmq_receiver.receive();
    cout << "Received aggregated data" << endl;

    payload::TaskResponse aggTaskResponseProto;
    aggTaskResponseProto.ParseFromString(rcvdData);
    return TaskResponse(aggTaskResponseProto.modelstatedict());
}
