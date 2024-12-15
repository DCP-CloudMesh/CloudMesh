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
    client->setupConn(bootstrapHost, bootstrapPort, "tcp");

    shared_ptr<Registration> payload = make_shared<Registration>();
    Message msg(uuid, IpAddress(host, port), payload);

    client->sendMsg(msg.serialize().c_str());
}

void Provider::listen() {
    while (true) {
        cout << "Waiting for requester to connect..." << endl;
        if (!server->acceptConn())
            continue;

        // receive task request object from client
        string requesterData = server->receiveFromConn();
        Message requesterMsg;
        requesterMsg.deserialize(requesterData);
        string requesterUuid = requesterMsg.getSenderUuid();
        IpAddress requesterIpAddr = requesterMsg.getSenderIpAddr();
        shared_ptr<Payload> requesterPayload = requesterMsg.getPayload();

        if (requesterPayload->getType() != Payload::Type::TASK_REQUEST) {
            continue;
        }

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

        server->closeConn();

        if (taskRequest->getLeaderUuid() == uuid) {
            leaderHandleTaskRequest(requesterIpAddr);
        } else {
            followerHandleTaskRequest();
        }
    }
}

void Provider::leaderHandleTaskRequest(const IpAddress& requesterIpAddr) {
    // Run processWorkload() in a separate thread
    thread workloadThread(&Provider::processWorkload, this);

    vector<string> followerData{};
    while (followerData.size() < taskRequest->getAssignedWorkers().size() - 1) {
        cout << "\nWaiting for follower peer to connect..." << endl;
        while (!server->acceptConn())
            ;

        // get data from followers and aggregate
        string followerMsgStr = server->receiveFromConn();
        Message followerMsg;
        followerMsg.deserialize(followerMsgStr);
        shared_ptr<Payload> followerPayload = followerMsg.getPayload();

        if (followerPayload->getType() != Payload::Type::TASK_RESPONSE) {
            continue;
        }

        shared_ptr<TaskResponse> taskResp =
            static_pointer_cast<TaskResponse>(followerPayload);

        // append to followerData
        cout << "task response: " << taskResp->getTrainingData() << endl;
        followerData.push_back(taskResp->getTrainingData());
        server->replyToConn("Received follower result.");
        server->closeConn();
    }

    cout << endl;
    workloadThread.join();
    TaskResponse aggregatedResults = aggregateResults(followerData);

    // Send results back to requester
    // TODO: requester IP address could change
    shared_ptr<TaskResponse> aggregatePayload =
        make_shared<TaskResponse>(aggregatedResults);
    Message aggregateResultMsg(uuid, IpAddress(host, port), aggregatePayload);

    cout << "Waiting for connection back to requester" << endl;
    // Keep trying to send results back to requester
    while (true) {
        // busy wait until connection is established
        while (client->setupConn(requesterIpAddr, "tcp") != 0) {
            sleep(5);
        }
        if (client->sendMsg(aggregateResultMsg.serialize().c_str()) != 0) {
            continue;
        }

        if (!server->acceptConn())
            continue;

        // receive response from requester
        string serializedData = server->receiveFromConn();
        server->closeConn();
        // process this request
        Message msg;
        msg.deserialize(serializedData);
        shared_ptr<Payload> payload = msg.getPayload();

        if (payload->getType() == Payload::Type::ACKNOWLEDGEMENT) {
            break;
        }
    }
}

void Provider::followerHandleTaskRequest() {
    processWorkload();
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

    // keep trying to send results back to leader
    while (client->sendMsg(msg.serialize().c_str()) != 0) {
        sleep(5);
    }
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

void Provider::processWorkload() {
    string indexFile = ingestTrainingData();

    // send training data index file to the worker
    cout << "Working on training data index file: " << indexFile << endl;
    payload::TrainingData proto;
    proto.set_training_data_index_filename(indexFile);
    string serialized;
    proto.SerializeToString(&serialized);
    cout << "Sending training data index file to worker..." << endl;
    ml_zmq_sender.send(serialized);
    cout << "Waiting for processed data..." << endl;
    auto rcvdData = ml_zmq_receiver.receive();
    cout << "Received processed data" << endl;

    // store result in taskResponse->trainingdata
    taskResponse = make_unique<TaskResponse>(rcvdData);
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
        payload::AggregatorInputData* proto =
            new payload::AggregatorInputData();
        proto->set_modelstatedict(followerData[i]);
        string serialized;
        proto->SerializeToString(&serialized);
        aggregator_zmq_sender.send(serialized);
    }

    // receive aggregated data from the aggregator
    cout << "Waiting for aggregated data..." << endl;
    auto rcvdData = aggregator_zmq_receiver.receive();
    cout << "Received aggregated data" << endl;

    taskResponse = make_unique<TaskResponse>(rcvdData);
    return *taskResponse;
}
