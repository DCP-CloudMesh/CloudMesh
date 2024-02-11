
#include "include/Peers/bootstrap_node.h"
#include "include/Peers/provider.h"
#include "include/Peers/requester.h"
#include "include/utility.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    // const char* host = "";
    unsigned short port = 8080;
    string uuid = "1";

    if (argc >= 2) {
        port = atoi(argv[1]);
    }

    // PURELY FOR DEMO PURPOSES. FOLLOWER PEER IS HARD CODED TO PORT 8081
    if (port == 8081) {
        uuid = "2";
    }

    // BootstrapNode bootstrap
    vector<int> trainingData{2, 1, 4, 3, 6, 5, 9, 7, 8, 10};

#if defined(PROVIDER)
    cout << "Running as provider on port " << port << "." << endl;
    Provider p = Provider(port, uuid);
    p.listen();
#elif defined(REQUESTER)
    cout << "Running as requester." << endl;
    Requester r = Requester(port);

    string requestType = "c";
    if (argc >= 3) {
        requestType = argv[2];
    }

    if (requestType == "c") {
        TaskRequest request = TaskRequest(trainingData);
        r.set_task_request(request);

        // sets the leaderIP and providerPeers
        r.send_discovery_request();

        // sends the task request to the leader and provider peers
        r.send_task_request();
        cout << "Sent task request." << endl;
    } else if (requestType == "r") {
        TaskResponse response = r.get_results();
        cout << "Received response: " << response.serialize() << endl;
    }

#else
    cout << "Please specify either --provider or --requester flag."
         << endl;
#endif
}
