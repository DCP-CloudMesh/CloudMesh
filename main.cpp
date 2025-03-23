#include "include/Peers/bootstrap_node.h"
#include "include/Peers/provider.h"
#include "include/Peers/requester.h"
#include "include/utility.h"
#include <boost/program_options.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#define DEFAULT_PORT 8080
#define DEFAULT_WORKERS 2
#define DEFAULT_EPOCHS 10

using namespace std;

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    unsigned short port = DEFAULT_PORT;

    string uuid = uuid::generate_uuid_v4();

#if defined(BOOTSTRAP)
    unsigned int bootstrapPort = BootstrapNode::getServerIpAddr().port;
    cout << "Running as bootstrap node on port " << bootstrapPort << "."
         << endl;
    BootstrapNode b = BootstrapNode(uuid);
    b.listen();
#elif defined(PROVIDER)
    try {
        // Define available program argument options
        po::options_description desc("Allowed options");
        desc.add_options()
            ("port,p", po::value<unsigned short>()->default_value(DEFAULT_PORT), "set P2P server port")
            ("help,h", "produce help message")
        ;

        // Parse command-line arguments
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        //Handle options
        if (vm.count("help")) {
            cout << desc << endl;
            return 0;
        }

        port = vm["port"].as<unsigned short>();
    } catch (const exception& e) {
        cerr << "Error parsing program arguments: " << e.what() << endl;
        return 1;
    }

    cout << "Running as provider on port " << port << "." << endl;
    Provider p = Provider(port, uuid);
    p.registerWithBootstrap();
    p.listen();
#elif defined(REQUESTER)
    // Define available program argument options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("port,p", po::value<unsigned short>()->default_value(DEFAULT_PORT),
            "set P2P server port")
        ("mode,m", po::value<string>(),
            "set mode to run requester: 'c' for compute task or 'r' for receive results")
        ("workers,w", po::value<unsigned int>()->default_value(DEFAULT_WORKERS),
            "set number of workers to assign training task")
        ("epochs,e", po::value<unsigned int>()->default_value(DEFAULT_EPOCHS),
            "set number of epochs to run training task")
        ("help,h", "produce help message")
    ;

    // Parse command-line arguments
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //Handle options
    if (vm.count("help")) {
        cout << desc << endl;
        return 0;
    }

    port = vm["port"].as<unsigned short>();
    string mode = vm["mode"].as<string>();

    cout << "Running as requester on port " << port << "." << endl;
    Requester r = Requester(port);

    if (mode == "c") {
        unsigned int numRequestedWorkers = vm["workers"].as<unsigned int>();
        unsigned int numEpochs = vm["epochs"].as<unsigned int>();
        cout << "Creating training task for " << numRequestedWorkers
             << " workers and " << numEpochs << " epochs" << endl;
        TaskRequest request = TaskRequest(numRequestedWorkers, ".*\\.jpg$",
                                          numEpochs, TaskRequest::GLOB_PATTERN);
        r.setTaskRequest(request);
        // sends the task request to the leader and provider peers
        r.sendTaskRequest();
        cout << "Sent task request." << endl;
    } else if (mode == "r") {
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
