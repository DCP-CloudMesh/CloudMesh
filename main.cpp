#include "include/Peers/bootstrap_node.h"
#include "include/Peers/provider.h"
#include "include/Peers/requester.h"
#include "include/utility.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

// comment testing

int main(int argc, char* argv[]) {
    unsigned short port = 8080;

    string uuid = uuid::generate_uuid_v4();

    if (argc >= 2) {
        port = stoul(string(argv[1]));
    }

#if defined(BOOTSTRAP)
    unsigned int bootstrapPort = BootstrapNode::getServerIpAddr().port;
    cout << "Running as bootstrap node on port " << bootstrapPort << "."
         << endl;
    BootstrapNode b = BootstrapNode(uuid);
    b.listen();
#elif defined(PROVIDER)
    cout << "Running as provider on port " << port << "." << endl;
    Provider p = Provider(port, uuid);
    p.registerWithBootstrap();
    p.listen();
#elif defined(REQUESTER)
    cout << "Running as requester." << endl;
    Requester r = Requester(port);
    int numRequestedWorkers = 2;
    int numEpochs = 2;

    string requestType = "c";
    if (argc >= 3) {
        requestType = argv[2];
    }

    if (requestType == "c") {
        TaskRequest request = TaskRequest(numRequestedWorkers, ".*\\.jpg$",
                                          numEpochs, TaskRequest::GLOB_PATTERN);
        r.setTaskRequest(request);
        // sends the task request to the leader and provider peers
        r.sendTaskRequest();
        cout << "Sent task request." << endl;
    } else if (requestType == "r") {
        TaskResponse response = r.getResults();
        std::string result = response.getTrainingData();
        cout << "Received training results" << endl;

        // output result.modelStateDict to a file as a binary
        std::filesystem::create_directories("output");
        std::ofstream file("output/modelStateDict.bin", std::ios::binary);

        file.write(reinterpret_cast<const char*>(result.data()), result.size());
        file.close();

        std::cout << "Successfully wrote to output/modelStateDict" << endl;
    }

#else
    cout << "Please specify either --provider or --requester flag." << endl;
#endif
}
