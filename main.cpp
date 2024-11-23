#include "include/Peers/bootstrap_node.h"
#include "include/Peers/provider.h"
#include "include/Peers/requester.h"
#include "include/utility.h"
#include <iostream>
#include <string>

using namespace std;

// comment testing

int main(int argc, char* argv[]) {
    const char* port = "8080";
    string uuid = uuid::generate_uuid_v4();

    if (argc >= 2) {
        port = argv[1];
    }

#if defined(BOOTSTRAP)
    cout << "Running as bootstrap node on port " << port << "." << endl;
    BootstrapNode b = BootstrapNode(port, uuid);
    b.listen();
#elif defined(PROVIDER)
    cout << "Running as provider on port " << port << "." << endl;
    Provider p = Provider(port, uuid);
    p.registerWithBootstrap();
    p.listen();
#elif defined(REQUESTER)
    cout << "Running as requester." << endl;
    Requester r = Requester(port);
    int numRequestedWorkers = 3;

    string requestType = "c";
    if (argc >= 3) {
        requestType = argv[2];
    }

    if (requestType == "c") {
        vector<int> trainingData{2, 1, 4, 3, 6, 5, 9, 7, 8, 10};
        TaskRequest request = TaskRequest(numRequestedWorkers, trainingData);
        r.setTaskRequest(request);
        // sends the task request to the leader and provider peers
        r.sendTaskRequest();
        cout << "Sent task request." << endl;
    } else if (requestType == "r") {
        TaskResponse response = r.getResults();
        cout << "Received response: " << response.serialize() << endl;
    }

#else
    cout << "Please specify either --provider or --requester flag." << endl;
#endif
}
