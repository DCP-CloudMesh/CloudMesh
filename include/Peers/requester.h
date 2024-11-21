#ifndef _REQUESTER_H_
#define _REQUESTER_H_

#include "../Networking/client.h"
#include "../RequestResponse/task_request.h"
#include "../RequestResponse/task_response.h"
#include "../utility.h"
#include "peer.h"
#include <sstream>
#include <vector>

class Requester : protected Peer {
    std::vector<TaskRequest> taskRequests;
    void divideTask();

  public:
    Requester(const char* port);
    ~Requester() noexcept;
    void sendDiscoveryRequest(unsigned int numProviders);
    void waitForDiscoveryResponse();
    void setTaskRequest(TaskRequest taskRequest);
    void sendTaskRequest();
    void checkStatus();
    TaskResponse getResults();
};

#endif // _REQUESTER_H_
