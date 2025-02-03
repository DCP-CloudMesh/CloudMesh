#include "../../include/Peers/provider.h"
#include "../../include/Peers/bootstrap_node.h"
#include "../../include/RequestResponse/acknowledgement.h"
#include "../../include/RequestResponse/message.h"
#include "../../include/RequestResponse/registration.h"
#include "../../include/RequestResponse/registration_response.h"
#include "../../include/RequestResponse/task_request.h"
#include "../../include/utility.h"
#include "proto/payload.pb.h"

using namespace std;

Provider::Provider(unsigned short port, string uuid)
    : Peer(uuid), ml_zmq_sender(), ml_zmq_receiver(), aggregator_zmq_sender(),
      aggregator_zmq_receiver() {
    isBusy = false;
    isLocalBootstrap = false;

    cout << "ML ZMQ: Sender: " << ml_zmq_sender.getAddress()
         << ", Receiver: " << ml_zmq_receiver.getAddress() << endl;
    cout << "Aggregator ZMQ: Sender: " << aggregator_zmq_sender.getAddress()
         << ", Receiver: " << aggregator_zmq_receiver.getAddress() << endl;

    setupServer(IpAddress("127.0.0.1", port));
}
Provider::~Provider() noexcept {}

void Provider::registerWithBootstrap() {
    IpAddress bootstrapIp = BootstrapNode::getServerIpAddr();
    cout << "Connecting to bootstrap node at " << bootstrapIp << endl;
    if (client->setupConn(bootstrapIp, "tcp") == -1) {
        cerr << "Unable to connect to boostrap node" << endl;
        exit(1);
    }

    shared_ptr<Registration> payload = make_shared<Registration>();
    Message msg(uuid, publicIp, payload);

    client->sendMsg(msg.serialize(), -1);

    // Get own public address from bootstrap node
    while (!server->acceptConn())
        ;

    string registrationRespStr;
    if (server->receiveFromConn(registrationRespStr) == 1) {
        cerr << "Failed to receive registration response" << endl;
        server->closeConn();
        exit(1);
    }

    Message respMsg;
    respMsg.deserialize(registrationRespStr);
    shared_ptr<Payload> respPayload = respMsg.getPayload();
    if (respPayload->getType() == Payload::Type::REGISTRATION_RESPONSE) {
        shared_ptr<RegistrationResponse> rr =
            static_pointer_cast<RegistrationResponse>(respPayload);
        IpAddress publicIp = rr->getCallerPublicIpAddress();
        cout << "Public IP = " << publicIp << endl;
        server->replyToConn("Obtained public ip address");
        setPublicIp(publicIp);
    }
    server->closeConn();
}

void Provider::listen() {
    while (true) {
        cout << "Waiting for requester to connect..." << endl;
        if (!server->acceptConn()) {
            continue;
        }

        // receive task request object from client
        string requesterData;
        if (server->receiveFromConn(requesterData) == 1) {
            server->closeConn();
            continue;
        }

        Message requesterMsg;
        requesterMsg.deserialize(requesterData);
        if (requesterMsg.getPayload()->getType() !=
            Payload::Type::TASK_REQUEST) {
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
        // Note: index files are downloaded to TARGET_INDEX_DATA_DIR
        server->getFileIntoDirFTP(taskRequest->getTrainingDataIndexFilename(),
                           TARGET_INDEX_DATA_DIR);

        // bug here where we are saving the file to the same file
        // fix in PR. RN, this will not work if multiple machines.
        // server->getFileFTP(taskRequest->getTrainingDataIndexFilename());
        ingestTrainingData();
        server->closeConn();

        // initialize ML.py with metadata
        workloadThread = new thread(&Provider::initializeWorkloadToML, this);

        if (taskRequest->getLeaderUuid() == uuid) {
            leaderHandleTaskRequest(requesterIpAddr);
        } else {
            followerHandleTaskRequest();
        }
    }
}

void Provider::leaderHandleTaskRequest(const IpAddress& requesterIpAddr) {
    TaskResponse aggregatedResults = TaskResponse();

    for (unsigned int i = 0; i < taskRequest->getNumEpochs(); i++) {
        vector<shared_ptr<TaskResponse>> followerData{};
        while (followerData.size() <
               taskRequest->getAssignedWorkers().size() -
                   1 // TODO: this needs to be changed when we change to
                     // multiple aggr. per epoch. There is an edge case where
                     // different followers could have different aggr. cycles
                     // required. So we can't wait for all followers to convene.
        ) {

            cout << "\nWaiting for follower peer to connect..." << endl;
            while (!server->acceptConn())
                ;

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

            // append to followerData
            shared_ptr<TaskResponse> taskResp =
                static_pointer_cast<TaskResponse>(followerPayload);
            followerData.push_back(taskResp);

            server->replyToConn("Received follower result.");
            server->closeConn();
        }

        cout << endl;

        workloadThread->join();
        delete workloadThread;

        // Aggregate model parameters and send to aggregator script
        aggregatedResults = aggregateResults(followerData);

        // if final cycle, we send back to requester
        if (aggregatedResults.getTrainingIsComplete()) {
            break;
        }

        // Send/Process aggregated results to follower peers
        cout << "Sending aggregated results to followers..." << endl;
        for (const auto& follower : taskRequest->getAssignedWorkers()) {
            if (follower.first == uuid) {
                // if the follower is the leader, skip sending

                // TODO: send the aggregated state dict to ML.py
                // Create payload containing a ModelStateDictParams message
                currentAggregatedModelStateDict =
                    aggregatedResults.getTrainingData();
                workloadThread = new thread(&Provider::processWorkload, this);
                continue;
            }

            IpAddress followerIp = follower.second;
            while (client->setupConn(followerIp, "tcp") != 0) {
                cout << "Failed to connect to follower to send aggregated "
                        "results, trying again in 5s"
                     << endl;
                sleep(5);
            }

            shared_ptr<TaskResponse> payload =
                make_shared<TaskResponse>(aggregatedResults);
            Message msg(uuid, publicIp, payload);
            int code = client->sendMsg(msg.serialize(), -1);
            cout << "Leader sent data to follower with code " << code << endl;
        }
    }

    // Send results back to requester
    // TODO: requester IP address could change
    shared_ptr<TaskResponse> aggregatePayload =
        make_shared<TaskResponse>(aggregatedResults);
    Message aggregateResultMsg(uuid, publicIp, aggregatePayload);

    // busy wait until connection is established
    while (client->setupConn(requesterIpAddr, "tcp") != 0) {
        constexpr int retry = 5;
        cout << "Failed to connect to requester server, trying again in "
             << retry << "s" << endl;
        sleep(retry);
    }
    cout << "Connected to requester server" << endl;

    if (client->sendMsg(aggregateResultMsg.serialize(), 5) == 1) {
        cerr << "Failed to send aggregated result to requester" << endl;
        return;
    }

    cout << "Sent results to requester. Waiting for acknowledgement" << endl;
    while (!server->acceptConn())
        ;

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

        server->replyToConn("Received acknowledgement.");
    }

    server->closeConn();
}

void Provider::followerHandleTaskRequest() {
    workloadThread->join();
    delete workloadThread;
    workloadThread = nullptr;

    for (unsigned int i = 0; i < taskRequest->getNumEpochs(); i++) {
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
        Message msg(uuid, publicIp, payload);
        int code = client->sendMsg(msg.serialize(), -1);
        cout << "Follower sent data to leader with code " << code << endl;

        if (i != taskRequest->getNumEpochs() - 1) {
            // TODO: change when we do multiple aggr. cycles in 1 epoch
            // wait for leader to send model state dict param
            while (!server->acceptConn())
                ;
            // receive aggregated TaskResponse object
            string leaderMsgStr;
            if (server->receiveFromConn(leaderMsgStr) == 1) {
                cerr << "Failed to receive aggregated TaskResponse object from "
                        "leader"
                     << endl;
                server->closeConn();
                continue;
            }

            Message leaderMsg;
            leaderMsg.deserialize(leaderMsgStr);
            shared_ptr<Payload> leaderPayload = leaderMsg.getPayload();

            if (leaderPayload->getType() != Payload::Type::TASK_RESPONSE) {
                server->closeConn();
                cerr << "Received payload is not of type TASK_RESPONSE" << endl;
                continue;
            }

            shared_ptr<TaskResponse> taskResp =
                static_pointer_cast<TaskResponse>(leaderPayload);
            taskResponse = make_shared<TaskResponse>(std::move(*taskResp));
            server->replyToConn("Received leader result.");
            server->closeConn();

            currentAggregatedModelStateDict = taskResponse->getTrainingData();

            // forward model state dict param to ml
            processWorkload();
        }
    }
}

void Provider::ingestTrainingData() {
    string trainingDataIndexFile = taskRequest->getTrainingDataIndexFilename();
    vector<string> requiredTrainingFiles =
        taskRequest->getTrainingDataFiles(TARGET_INDEX_DATA_DIR);
    cout << "Task requires " << requiredTrainingFiles.size()
         << " training files" << endl;
    // Ensure all required training data files are present
    for (const string& filename : requiredTrainingFiles) {
        if (!isFileWithinDirectory(filename, TARGET_TRAINING_DATA_DIR)) {
            cout << "FTP: requesting " << filename << endl;
            server->getFileIntoDirFTP(filename, TARGET_TRAINING_DATA_DIR);
        }
    }
    cout << "All training files are now present" << endl;
}

void Provider::processWorkload() {
    // send the current model state dict to ML.py
    payload::ModelStateDictParams modelStateDictParamsProto;
    modelStateDictParamsProto.set_modelstatedict(
        currentAggregatedModelStateDict);
    string serialized_modelStateDictParamsProto;
    modelStateDictParamsProto.SerializeToString(
        &serialized_modelStateDictParamsProto);
    // TODO: send model state dict param to ML.py
    ml_zmq_sender.send(serialized_modelStateDictParamsProto);

    cout << "Waiting for processed data..." << endl;
    auto rcvdData = ml_zmq_receiver.receive();
    cout << "Received processed data" << endl;

    // Parse received task response proto and populate TaskResponse object
    payload::TaskResponse task_response_proto;
    task_response_proto.ParseFromString(rcvdData);
    taskResponse =
        make_shared<TaskResponse>(task_response_proto.modelstatedict(),
                                  task_response_proto.trainingiscomplete());
    cout << "Completed assigned workload" << endl;
}

void Provider::initializeWorkloadToML() {
    // send training data index file to the worker
    cout << "Sending training data index file to worker..." << endl;
    payload::TrainingData training_data_proto;
    training_data_proto.set_training_data_index_filename(
        taskRequest->getTrainingDataIndexFilename());
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
    taskResponse =
        make_shared<TaskResponse>(task_response_proto.modelstatedict(),
                                  task_response_proto.trainingiscomplete());
    cout << "Completed assigned workload" << endl;
}

TaskResponse
Provider::aggregateResults(vector<shared_ptr<TaskResponse>> followerData) {
    cout << "Aggregating results..." << endl;

    // append leader's data to back of followerData
    followerData.push_back(taskResponse);

    // send each follower's data to the aggregator
    cout << "Sending data to aggregator..." << endl;
    for (unsigned int i = 0; i < followerData.size(); i++) {
        cout << "Aggregator for loop: " << i << endl;
        payload::ModelStateDictParams* proto =
            new payload::ModelStateDictParams();
        proto->set_modelstatedict(followerData[i]->getTrainingData());
        proto->set_trainingiscomplete(followerData[i]->getTrainingIsComplete());
        string serialized;
        proto->SerializeToString(&serialized);
        aggregator_zmq_sender.send(serialized);
        delete proto;
    }

    // receive aggregated data from the aggregator
    cout << "Waiting for aggregated data..." << endl;
    auto rcvdData = aggregator_zmq_receiver.receive();
    cout << "Received aggregated data" << endl;

    payload::TaskResponse aggTaskResponseProto;
    aggTaskResponseProto.ParseFromString(rcvdData);
    TaskResponse aggTaskResponse =
        TaskResponse(aggTaskResponseProto.modelstatedict(),
                     aggTaskResponseProto.trainingiscomplete());
    return aggTaskResponse;
}
