#ifndef _PROVIDER_H_
#define _PROVIDER_H_

#include "../IPC/zmq_receiver.h"
#include "../IPC/zmq_sender.h"
#include "../Networking/client.h"
#include "../Networking/server.h"
#include "../RequestResponse/task_request.h"
#include "../RequestResponse/task_response.h"

#include "peer.h"
#include <memory>
#include <thread>
#include <vector>

class Provider : public Peer {
    bool isBusy;
    bool isLocalBootstrap;
    bool isLeader;
    std::unique_ptr<TaskRequest> taskRequest;
    std::unique_ptr<TaskResponse> taskResponse;
    ZMQSender zmq_sender;
    ZMQReceiver zmq_receiver;

  public:
    Provider(const char* port, std::string uuid, const char* zmq_port);
    ~Provider() noexcept;

    void registerWithBootstrap();
    void listen();
    void leaderHandleTaskRequest(const IpAddress& requesterIpAddr);
    void followerHandleTaskRequest();
    void processData();
    void processWorkload(); // worker function to manipulate the TaskRequest
    std::vector<int>
    ingestTrainingData(); // worker function to load training data into memory
    TaskResponse aggregateResults(std::vector<std::vector<int>> followerData);
};

#endif
