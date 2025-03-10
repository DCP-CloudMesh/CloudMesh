#include "../../include/Peers/requester.h"
#include "../../include/Peers/bootstrap_node.h"
#include "../../include/RequestResponse/acknowledgement.h"
#include "../../include/RequestResponse/discovery_request.h"
#include "../../include/RequestResponse/discovery_response.h"
#include "../../include/RequestResponse/message.h"
#include "../../include/RequestResponse/task_request.h"
#include "../../include/utility.h"

#include <algorithm>
#include <random>

using namespace std;

Requester::Requester(const char* port) : Peer() {
    setupServer("127.0.0.1", port);
}

Requester::~Requester() noexcept {}

void Requester::sendDiscoveryRequest(unsigned int numProviders) {
    const char* bootstrapHost = BootstrapNode::getServerIpAddress();
    const char* bootstrapPort = BootstrapNode::getServerPort();
    cout << "Connecting to bootstrap node at " << bootstrapHost << ":"
         << bootstrapPort << endl;
    if (client->setupConn(bootstrapHost, bootstrapPort, "tcp") == -1) {
        cerr << "Unable to connect to boostrap node" << endl;
        exit(1);
    }

    shared_ptr<Payload> payload = make_shared<DiscoveryRequest>(numProviders);
    Message msg(uuid, IpAddress(host, port), payload);
    client->sendMsg(msg.serialize(), -1);
}

void Requester::waitForDiscoveryResponse() {
    cout << "Waiting for discovery response..." << endl;
    while (!server->acceptConn());

    // receive response from bootstrap (or possibly another peer)
    string serializedData;
    if (server->receiveFromConn(serializedData) == 1) {
        cerr << "Failed to receive discovery response" << endl;
        server->closeConn();
        exit(1);
    }

    // process response
    string replyPrefix = "Requester (" + uuid + ") - ";
    Message msg;
    msg.deserialize(serializedData);
    shared_ptr<Payload> payload = msg.getPayload();

    if (payload->getType() == Payload::Type::DISCOVERY_RESPONSE) {
        shared_ptr<DiscoveryResponse> dr =
            static_pointer_cast<DiscoveryResponse>(payload);
        AddressTable availablePeers = dr->getAvailablePeers();
        for (auto& it : availablePeers) {
            providerPeers[it.first] = it.second;
        }
        string logMsg = "Received " + to_string(availablePeers.size()) +
                        " peers in discovery response";
        cout << logMsg << endl;
        server->replyToConn(replyPrefix + logMsg);
    }
    server->closeConn();
}

void Requester::setTaskRequest(TaskRequest request_) {
    // push large task request into the vector
    taskRequests.push_back(request_);
}

void Requester::divideTask() {
    TaskRequest queuedTask = taskRequests.front();

    // obtain list of training files
    vector<string> trainingFiles = queuedTask.getTrainingDataFiles();
    cout << "Found " << trainingFiles.size() << " training files" << endl;

    // // shuffle training files for even distribution after division
    // std::random_device rd; // Seed generator
    // std::mt19937 g(rd());  // Mersenne Twister engine
    // std::shuffle(trainingFiles.begin(), trainingFiles.end(), g);

    // divide the vector into subvectors
    int numSubtasks = queuedTask.getNumWorkers();
    int numEpochs = queuedTask.getNumEpochs();
    int subtaskSize = trainingFiles.size() / numSubtasks;
    int remainder = trainingFiles.size() % numSubtasks;

    cout << "Dividing task into " << numSubtasks << " subtasks" << endl;
    cout << "Subtask size: " << subtaskSize << endl;
    cout << "Remainder: " << remainder << endl;

    string leaderUuid = queuedTask.getLeaderUuid();
    AddressTable assignedWorkers = queuedTask.getAssignedWorkers();

    // create subvectors of requesttasks
    vector<TaskRequest> subtasks;
    for (int i = 0; i < numSubtasks; i++) {
        vector<string> subtaskTrainingFiles;
        int currentSubtaskSize = subtaskSize;

        if (i == numSubtasks - 1) {
            currentSubtaskSize += remainder;
        }

        for (int j = 0; j < currentSubtaskSize; j++) {
            subtaskTrainingFiles.push_back(trainingFiles[i * subtaskSize + j]);
        }

        // Build a path by combining the filename and DATA_DIR using path joins
        string filename = "subtaskIndex_" + std::to_string(i) + ".txt";
        TaskRequest subtaskRequest(1, filename, numEpochs, TaskRequest::INDEX_FILENAME);
        subtaskRequest.writeToTrainingDataIndexFile(subtaskTrainingFiles);
        cout << "FTP: Created index file "
             << subtaskRequest.getTrainingDataIndexFilename() << endl;
        subtaskRequest.setLeaderUuid(leaderUuid);
        subtaskRequest.setAssignedWorkers(assignedWorkers);
        taskRequests.push_back(subtaskRequest);
    }

    // remove the task request
    taskRequests.erase(taskRequests.begin());
}

void Requester::sendTaskRequest() {
    // Ensure connected to sufficient number of provider peers
    unsigned int numRequestedWorkers = taskRequests[0].getNumWorkers();
    while (providerPeers.size() < numRequestedWorkers) {
        cout << "Insufficient number of connected provider peers" << endl
             << "Sending out discovery request..." << endl;
        sendDiscoveryRequest(numRequestedWorkers);
        waitForDiscoveryResponse();
    }

    // Set task leader and followers
    AddressTable workers{};
    for (auto& provider : providerPeers) {
        if (workers.size() == numRequestedWorkers) {
            break;
        }
        workers[provider.first] = provider.second;
    }
    taskRequests[0].setAssignedWorkers(workers);
    taskRequests[0].setLeaderUuid(workers.begin()->first);

    // divides the task into subtasks
    divideTask();

    // send the task request to the workers
    int ctr = 0;
    for (auto& worker : workers) {
        // package and serialize the requests
        shared_ptr<TaskRequest> payload =
            make_shared<TaskRequest>(taskRequests[ctr]);
        Message msg(uuid, IpAddress(host, port), payload);

        // set up the client
        const char* host = worker.second.host.c_str();
        const char* port = to_string(worker.second.port).c_str();
        client->setupConn(host, port, "tcp");

        // send the request
        client->sendMsg(msg.serialize(), -1);
        ctr++;
    }
}

void Requester::checkStatus() {}

TaskResponse Requester::getResults() {
    TaskResponse taskResult;
    // busy wait until connection is established
    cout << "Waiting for leader peer to connect" << endl;
    while (!server->acceptConn());

    // get data from workers and aggregate
    cout << "Waiting for leader peer to send results" << endl;
    string serializedData;
    server->receiveFromConn(serializedData);
    server->replyToConn("Received task result.");
    server->closeConn();

    // deserialize into data
    Message msg;
    msg.deserialize(serializedData);
    IpAddress leaderIpAddr = msg.getSenderIpAddr();
    shared_ptr<TaskResponse> taskRespPtr =
        static_pointer_cast<TaskResponse>(msg.getPayload());
    taskResult = std::move(*taskRespPtr); // transfer data

    // send success acknowledgement to provider
    shared_ptr<Acknowledgement> payload = make_shared<Acknowledgement>();
    Message response(uuid, IpAddress(host, port), payload);
    client->setupConn(leaderIpAddr, "tcp");
    client->sendMsg(response.serialize());

    return taskResult;
}
