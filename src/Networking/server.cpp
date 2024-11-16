#include "../../include/Networking/server.h"

using namespace std;

Server::Server(const char* host, const char* port, const char* type)
    : HOST{host}, PORT{port}, CONNTYPE{type}, server{-1} {}

void Server::setupServer() {
#if defined(NOLOCAL)
    string response = startNgrokForwarding(stoi(PORT));
    // update public IP address
    response = response.substr(6);
    string ip =
        response.substr(0, response.find(":")); // ignore "tcp://" prefix
    unsigned short port =
        static_cast<unsigned short>(stoi(response.substr(ip.length() + 1)));
    publicIP = IpAddress{ip, port};
#elif defined(LOCAL)
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
        close(server);
        exit(1);
    }

    if (listen(server, 5) == -1) {
        cerr << "Error listening: " << strerror(errno) << endl;
        close(server);
        exit(1);
    }

    cout << "Server initialized" << endl;
}

bool Server::acceptConn() {
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    activeConn = accept(server, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if (activeConn == -1) {
        cerr << "Error accepting: " << strerror(errno) << endl;
        close(server);
        return false;
    }

    cout << "Client connected" << endl;
    return true;
}

string Server::receiveFromConn() {
    char buffer[1024];
    ssize_t mLen = recv(activeConn, buffer, sizeof(buffer), 0);
    if (mLen < 0) {
        cerr << "Error reading: " << strerror(errno) << endl;
    }
    return string(buffer, mLen);
}

void Server::replyToConn(string message) {
    const char* reply = message.c_str();
    send(activeConn, reply, strlen(reply), 0);
}

void Server::getFileFTP(string message) {
    const char* fileName = message.c_str();
    const char* reply = ("get " + message).c_str();
    cout << "FTP: sending request \"" << reply << "\"" << endl;
    send(activeConn, reply, strlen(reply), 0);

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
        if ((fp = fopen(fileName, "w")) == NULL)
            cout << "FTP: Error in creating file\n";
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
        cerr
            << "FTP: Error in opening file. Check filename\nUsage: put filename"
            << endl;
    }
}

void Server::closeConn() { close(activeConn); }

Server::~Server() {
    if (server != -1) {
        close(server);
    }
}
