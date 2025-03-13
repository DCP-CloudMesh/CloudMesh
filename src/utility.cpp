#include "../include/utility.h"

using namespace std;

IpAddress::IpAddress(const string& host, const unsigned short port)
    : host(host), port(port) {}

IpAddress::IpAddress(const char* host, const char* port) {
    this->host = host;
    string portStr(port);
    this->port = stoi(portStr);
}

// converts an IpAddress object to a utility::IpAddress proto object
utility::IpAddress* serializeIpAddressToProto(const IpAddress& ipAddress) {
    utility::IpAddress* j = new utility::IpAddress();
    j->set_ip(ipAddress.host);
    j->set_port(ipAddress.port);

    return j;
}

// converts from a string to the utility::IpAddress proto object
IpAddress deserializeIpAddressFromProto(const utility::IpAddress& proto) {
    IpAddress ip;

    ip.host = proto.ip();
    ip.port = proto.port();
    return ip;
}

// overloaded output operator for IpAddress
std::ostream& operator<<(std::ostream& os, const IpAddress& ip) {
    return os << ip.host << ":" << ip.port;
}

// converts a AddressTable object to a utility::AddressTable proto object
utility::AddressTable* serializeAddressTable(const AddressTable& addressTable) {
    utility::AddressTable* at = new utility::AddressTable();

    for (const auto& it : addressTable) {
        utility::IpAddress* protoIp = new utility::IpAddress();
        protoIp->set_ip(it.second.host);
        protoIp->set_port(it.second.port);
        at->mutable_address_table()->insert({it.first, *protoIp});
    }

    return at;
}

// converts from a utility::AddressTable proto object to the AddressTable object
AddressTable deserializeAddressTable(const utility::AddressTable at) {
    AddressTable addressTable;

    for (const auto& it : at.address_table()) {
        IpAddress ip(it.second.ip(), it.second.port());
        addressTable[it.first] = ip;
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

int get_available_port() {
    std::random_device random_num_generator;
    std::mt19937 gen(random_num_generator());
    std::uniform_int_distribution<> dis(MIN_PORT, MAX_PORT);

    for (int i = 0; i < MAX_PORT_TRIES; i++) {
        int port = dis(gen);
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            continue;

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (::bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            close(sock);
            return port;
        }

        close(sock);
    }
    return -1;
}

vector<string> getMatchingDataFiles(const regex& pattern,
                                    const string& directory) {
    vector<string> matchingDataFiles;
    for (const auto& entry :
         filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            const string filename = entry.path().filename().string();
            if (regex_match(filename, pattern)) {
                matchingDataFiles.push_back(filename);
            }
        }
    }

    return matchingDataFiles;
}

regex convertToRegexPattern(const string& pattern) {
    try {
        return regex(pattern, regex_constants::icase); // case insensitive
    } catch (const std::regex_error& e) {
        cerr << "Regex construction failed: " << e.what() << endl;
        return regex();
    }
}
