#include "../include/utility.h"
#include <nlohmann/json.hpp>

using namespace std;
using namespace nlohmann;

string serializeIpAddress(const IpAddress& ipAddress) {
    json j;
    j["ip"] = ipAddress.host;
    j["port"] = ipAddress.port;
    return j.dump();
}

IpAddress::IpAddress(const string& host, const unsigned short port)
    : host(host), port(port) {}

IpAddress::IpAddress(const char* host, const char* port) {
    this->host = host;
    string portStr(port);
    this->port = stoi(portStr);
}

IpAddress deserializeIpAddress(const string& jsonString) {
    IpAddress ipAddress;
    try {
        json j = json::parse(jsonString);
        ipAddress.host = j["ip"].get<string>();
        ipAddress.port = j["port"].get<unsigned short>();
    } catch (json::exception& e) {
        cout << "JSON parsing error for IpAddress: " << e.what() << endl;
    }
    return ipAddress;
}

string serializeAddressTable(const AddressTable& addressTable) {
    json j;
    for (auto& it : addressTable) {
        j[it.first] = serializeIpAddress(it.second);
    }
    return j.dump();
}

AddressTable deserializeAddressTable(const string& jsonString) {
    AddressTable addressTable;
    try {
        json j = json::parse(jsonString);
        for (auto it = j.begin(); it != j.end(); it++) {
            addressTable[it.key()] = deserializeIpAddress(it.value());
        }
    } catch (json::exception& e) {
        cout << "JSON parsing error for address table: " << e.what() << endl;
    }
    return addressTable;
}

string uuid::generate_uuid_v4() {
    stringstream ss;
    int i;
    ss << hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    };
    return ss.str();
}

string startNgrokForwarding(unsigned short port) {
    const string command =
        "python3 ./src/Networking/ngrok_ip.py " + to_string(port);

    // start ngrok
    string ngrok_restart =
        "./src/Networking/ngrok_restart.sh " + to_string(port) + " &";
    system(ngrok_restart.c_str());

    // Open a pipe to capture the output
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        cerr << "Error opening pipe." << endl;
        return "";
    }

    // read
    char buffer[256];
    string result = "";
    // while (fgets(buffer, 256, pipe) != nullptr) {
    //     result += buffer;
    // }

    // // Close the pipe
    // pclose(pipe);
    // return result;
    while (!feof(pipe)) {
        if (fgets(buffer, 256, pipe) != nullptr)
            result += buffer;
    }

    // Close the pipe
    pclose(pipe);
    return result;
}

void close_ngrok_forwarding() { system("pkill ngrok"); }

string vectorToString(vector<int> vec) {
    stringstream ss;
    for (unsigned int i = 0; i < vec.size(); i++) {
        ss << vec[i];
        if (i != vec.size() - 1) {
            ss << ",";
        }
    }

    return ss.str();
}

int FTP_create_socket_client(int port, const char* addr) {
    cout << "FTP: Creating FTP client socket " << port << " on " << addr
         << endl;

    const char* localhost = "127.0.0.1";
    int sockfd;
    struct sockaddr_in servaddr;

    // Create a socket for the client
    // If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "FTP: Problem in creating the socket" << endl;
        exit(2);
    }

    // Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(localhost);
    servaddr.sin_port = htons(port); // convert to big-endian order

    // Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        cerr << "FTP: Problem in creating data channel" << endl;
        exit(3);
    }

    return (sockfd);
}

int FTP_create_socket_server(int port) {
    cout << "FTP: Creating FTP server socket " << port << endl;
    int listenfd;
    struct sockaddr_in dataservaddr;

    // Create a socket for the soclet
    // If sockfd<0 there was an error in the creation of the socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "FTP: Problem in creating the data socket" << endl;
        exit(2);
    }

    // preparation of the socket address
    dataservaddr.sin_family = AF_INET;
    dataservaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    dataservaddr.sin_port = htons(port);

    if ((::bind(listenfd, (struct sockaddr*)&dataservaddr,
                sizeof(dataservaddr))) < 0) {
        cerr << "FTP: Problem in binding the data socket" << endl;
        exit(2);
    }

    // listen to the socket by creating a connection queue, then wait for
    // clients
    listen(listenfd, 1);

    return (listenfd);
}

int FTP_accept_conn(int sock) {
    int dataconnfd;
    socklen_t dataclilen;
    struct sockaddr_in datacliaddr;

    dataclilen = sizeof(datacliaddr);
    // accept a connection
    if ((dataconnfd =
             accept(sock, (struct sockaddr*)&datacliaddr, &dataclilen)) < 0) {
        cerr << "FTP: Problem in accepting the data socket" << endl;
        exit(2);
    }

    return (dataconnfd);
}

fs::path resolveDataFile(const std::string filename) {
    std::string resolvedFilename = DATA_DIR + "/" + filename;
    return fs::path(resolvedFilename);
}

bool isFileWithinDataDirectory(const std::string& filename) {
    std::regex cloudmeshDataPattern(".*cloudmesh/" + std::string(DATA_DIR) +
                                        ".*",
                                    std::regex_constants::icase);

    try {
        fs::path requestedPath = resolveDataFile(filename);
        std::string canonicalPathStr =
            fs::canonical(fs::absolute(requestedPath)).string();
        return std::regex_search(canonicalPathStr, cloudmeshDataPattern);
    } catch (const std::exception& e) {
        std::cerr << "Caught Error: " << e.what() << std::endl;
        return false; // Invalid path format or not within the data directory
    }
}
