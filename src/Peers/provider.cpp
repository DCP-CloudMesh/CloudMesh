#include "../../include/Peers/provider.h"
#include "../../include/Peers/bootstrap_node.h"
#include "../../include/RequestResponse/acknowledgement.h"
#include "../../include/RequestResponse/message.h"
#include "../../include/RequestResponse/registration.h"
#include "../../include/RequestResponse/task_request.h"
#include "../../include/utility.h"

#include <algorithm>
#include <boost/process.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace std;

Provider::Provider(const char* port, string uuid)
    : Peer(uuid), zmq_sender(), zmq_receiver() {
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

        // FTP
        shared_ptr<TaskRequest> taskReq =
            static_pointer_cast<TaskRequest>(requesterPayload);
        task = make_unique<TaskRequest>(std::move(*taskReq));

        if (task->getTrainingFile().empty()) {
            // if training file is not set, then we continue assuming the
            // trainingData field has been set
            server->replyToConn("Provider ID: " + uuid +
                                " : Deserialized "
                                "task request. Now processing workload.\n");
            server->closeConn();
        } else {
            // if training file is not empty, then download the training file
            // from the requester and set the trainingData field from the file
            cout << "FTP: requesting " << task->getTrainingFile() << endl;
            server->getFileFTP(task->getTrainingFile());

            // add the training data from the file fetched
            task->setTrainingDataFromFile();

            server->closeConn();
        }

        try {
            // Spin up python script child process
            const char* python_env = std::getenv("PYTHON_INTERPRETER");
            std::string python_interpreter = python_env ? python_env : "/usr/bin/python3";
            std::string script_path = "sorting_python/sorting.py";
            std::vector<std::string> args{
                std::to_string(zmq_sender.getPort()),
                std::to_string(zmq_receiver.getPort())};
            boost::process::child python_script(python_interpreter, script_path, args);

            // Process P2P communication
            if (task->getLeaderUuid() == uuid) {
                leaderHandleTaskRequest(requesterIpAddr);
            } else {
                followerHandleTaskRequest();
            }

            // Wait for python script to terminate
            python_script.wait();

            int exit_code = python_script.exit_code();
            if (exit_code != 0) {
                std::cout << "Error: Python child process exited with code: " << exit_code << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }
}

void Provider::leaderHandleTaskRequest(const IpAddress& requesterIpAddr) {
    // Run processWorkload() in a separate thread
    thread workloadThread(&Provider::processWorkload, this);

    vector<vector<int>> followerData{};
    while (followerData.size() < task->getAssignedWorkers().size() - 1) {
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
    IpAddress leaderIp = task->getAssignedWorkers()[task->getLeaderUuid()];
    // busy wait until connection is established with the leader
    while (client->setupConn(leaderIp, "tcp") == -1) {
        sleep(5);
    }

    // send results back to leader
    shared_ptr<TaskResponse> payload =
        make_shared<TaskResponse>(task->getTrainingData());
    Message msg(uuid, IpAddress(host, port), payload);

    // keep trying to send results back to leader
    while (client->sendMsg(msg.serialize().c_str()) != 0) {
        sleep(5);
    }
}

void Provider::processWorkload() {
    // data is stored in task->training data
    vector<int> data = task->getTrainingData();

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

    // store result in task->trainingdata
    task->setTrainingData(data);
    cout << "Completed assigned workload" << endl;
}

TaskResponse Provider::aggregateResults(vector<vector<int>> followerData) {
    vector<int> curr_data = task->getTrainingData();
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

    task->setTrainingData(data);
    TaskResponse taskResponse(data);
    return taskResponse;
}
