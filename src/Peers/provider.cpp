#include "../../include/Peers/provider.h"
#include "../../include/Peers/bootstrap_node.h"
#include "../../include/RequestResponse/acknowledgement.h"
#include "../../include/RequestResponse/message.h"
#include "../../include/RequestResponse/registration.h"
#include "../../include/RequestResponse/task_request.h"
#include "../../include/utility.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace std;

Provider::Provider(const char* port, string uuid, const char* zmq_port)
    : Peer(uuid), zmq_sender(stoi(zmq_port)), zmq_receiver(stoi(zmq_port) + 1) {
    isBusy = false;
    isLocalBootstrap = false;

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
        server->getFileFTP(taskRequest->getTrainingDataIndexFilename());

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

    vector<vector<int>> followerData{};
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

vector<int> Provider::ingestTrainingData() {
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

    // read content in the training data files
    vector<int> data;
    for (const string& filename : requiredTrainingFiles) {
        ifstream file(resolveDataFile(filename));
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            exit(1);
        }

        string line;
        while (getline(file, line)) {
            data.push_back(stoi(line));
        }
        file.close();
    }

    return data;
}

void Provider::processWorkload() {
    vector<int> data = ingestTrainingData();

    // turn the vector into a string
    string dataStr = "";
    for (int i = 0; i < data.size(); i++) {
        dataStr += to_string(data[i]);
        if (i != data.size() - 1) {
            dataStr += ",";
        }
    }

    // send data to the worker
    cout << "Unprocessed workload with data: " << dataStr << endl;
    zmq_sender.send(dataStr);
    cout << "Waiting for processed data..." << endl;
    auto rcvdData = zmq_receiver.receive();

    // turn the string back into a vector
    data.clear();
    string num = "";
    for (int i = 0; i < rcvdData.size(); i++) {
        if (rcvdData[i] == ',') {
            data.push_back(stoi(num));
            num = "";
        } else {
            num += rcvdData[i];
        }
    }
    data.push_back(stoi(num));
    cout << "Processed workload with data: ";
    for (int i = 0; i < data.size(); i++) {
        cout << data[i] << " ";
    }
    cout << endl;

    // store result in taskResponse->trainingdata
    taskResponse = make_unique<TaskResponse>(data);
    cout << "Completed assigned workload" << endl;
}

TaskResponse Provider::aggregateResults(vector<vector<int>> followerData) {
    vector<int> curr_data = taskResponse->getTrainingData();
    followerData.push_back(curr_data);

    vector<int> indicies(followerData.size(), 0);
    vector<int> data;

    while (true) {
        int minVal = INT_MAX;
        int minIdx = -1;
        for (int i = 0; i < followerData.size(); i++) {
            if (indicies[i] < followerData[i].size() &&
                followerData[i][indicies[i]] < minVal) {
                minVal = followerData[i][indicies[i]];
                minIdx = i;
            }
        }

        if (minIdx == -1) {
            break;
        }

        data.push_back(minVal);
        indicies[minIdx]++;
    }

    cout << "Aggregated results: ";
    for (int i = 0; i < data.size(); i++) {
        cout << data[i] << " ";
    }

    taskResponse->setTrainingData(data);
    return *taskResponse;
}
