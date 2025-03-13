#include "../../include/Networking/server.h"

using namespace std;

Server::Server(const char* host, const char* port, const char* type)
    : HOST{host}, PORT{port}, CONNTYPE{type}, server{-1} {}

void Server::setupServer() {
#if defined(NOLOCAL) || defined(LOCAL)
    publicIP = IpAddress{HOST, static_cast<unsigned short>(stoi(PORT))};
#else
    cerr << "Please specify either --local or --nolocal flag." << endl;
    exit(1);
#endif

    cout << "Initializing server on " << publicIP.host << ":" << publicIP.port
         << endl;

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        cerr << "Error creating socket: " << strerror(errno) << endl;
        exit(1);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(stoi(PORT));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(server, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) ==
        -1) {
        cerr << "Error binding: " << strerror(errno) << endl;
        closeSocket();
        exit(1);
    }

    if (listen(server, 5) == -1) {
        cerr << "Error listening: " << strerror(errno) << endl;
        closeSocket();
        exit(1);
    }

    cout << "Server initialized" << endl;
}

bool Server::acceptConn() {
    IpAddress addr;
    return acceptConn(addr);
}

bool Server::acceptConn(IpAddress& clientAddr) {
    sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    activeConn = accept(server, (struct sockaddr*)&addr, &addrLen);

    if (activeConn == -1) {
        cerr << "Error accepting: " << strerror(errno) << endl;
        return false;
    }

    // Get address of incoming connection
    char addrBuffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,  &addr.sin_addr, addrBuffer, sizeof(addrBuffer));
    clientAddr.host = string(addrBuffer);
    clientAddr.port = htons(addr.sin_port);

    cout << "Client connected from " << clientAddr.host << ":" << clientAddr.port << endl;
    return true;
}

ssize_t Server::recvAllBytes(char* buffer, size_t length, int flags, int num_retries) {
    size_t total_received = 0;
    int retries_used = 0;
    while (total_received < length) {
        ssize_t bytes_received = recv(activeConn, buffer + total_received,
                                     length - total_received, flags);

        if (bytes_received == 0) {
            cerr << "Connection closed by client" << endl;
            return -1;
        } else if (bytes_received == -1) {
            if (num_retries == -1 || retries_used < num_retries) {
                retries_used++;
                continue;
            }
            cerr << "Failed to receive all bytes. " << total_received << "/"
                 << length << " bytes received after " << retries_used
                 << " retries." << endl;
            return -1;
        }

        total_received += bytes_received;
    }
    return total_received;
}

int Server::receiveFromConn(string& msg, int num_retries) {
    // Read message size
    uint32_t data_size;
    if (recvAllBytes(reinterpret_cast<char*>(&data_size), sizeof(data_size), 0, num_retries) == -1) {
        cerr << "Failed to receive message length" << endl;
        return 1;
    }
    data_size = ntohl(data_size); // Convert from network byte order

    // Read message data
    string data(data_size, '\0');
    if (recvAllBytes(data.data(), data_size, 0, num_retries) == -1) {
        cerr << "Failed to receive message data" << endl;
        return 1;
    }

    msg = data;

    return 0;
}

void Server::replyToConn(string message) {
    const char* reply = message.c_str();
    send(activeConn, reply, strlen(reply), 0);
}

void Server::getFileFTP(string filename) {
    std::string reply = "get " + filename;
    cout << "FTP: sending request \"" << reply << "\"" << endl;
    send(activeConn, reply.c_str(), strlen(reply.c_str()), 0);

    char port[FTP_BUFFER_SIZE], buffer[FTP_BUFFER_SIZE],
        char_num_blks[FTP_BUFFER_SIZE], char_num_last_blk[FTP_BUFFER_SIZE],
        msg[FTP_BUFFER_SIZE];
    int data_port, datasock, lSize, num_blks, num_last_blk, i;
    FILE* fp;
    recv(activeConn, port, FTP_BUFFER_SIZE, 0);
    data_port = atoi(port);
    datasock = FTP_create_socket_client(data_port, PORT);
    recv(activeConn, msg, FTP_BUFFER_SIZE, 0);
    if (strcmp("nxt", msg) == 0) {
        if ((fp = fopen(resolveDataFile(filename).c_str(), "w")) == NULL)
            cout << "FTP: Error in creating file" << endl;
        else {
            recv(activeConn, char_num_blks, FTP_BUFFER_SIZE, 0);
            num_blks = atoi(char_num_blks);
            for (i = 0; i < num_blks; i++) {
                recv(datasock, buffer, FTP_BUFFER_SIZE, 0);
                fwrite(buffer, sizeof(char), FTP_BUFFER_SIZE, fp);
            }
            recv(activeConn, char_num_last_blk, FTP_BUFFER_SIZE, 0);
            num_last_blk = atoi(char_num_last_blk);
            if (num_last_blk > 0) {
                recv(datasock, buffer, FTP_BUFFER_SIZE, 0);
                fwrite(buffer, sizeof(char), num_last_blk, fp);
            }
            fclose(fp);
            cout << "FTP: File download done." << endl;
        }
    } else {
        cerr << "FTP: Error in opening file. Check filename" << endl;
    }
}

void Server::closeConn() {
    if (activeConn != -1) {
        close(activeConn);
        activeConn = -1;
    }
}

void Server::closeSocket() {
    if (server != -1) {
        closeConn();
        close(server);
        server = -1;
    }
}

Server::~Server() {
    closeSocket();
}
