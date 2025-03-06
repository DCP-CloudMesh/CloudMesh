#include "../../include/Networking/client.h"

using namespace std;

Client::Client() : CONN{-1} {}

Client::~Client() {}

int Client::setupConn(const char* HOST, const char* PORT,
                      const char* CONNTYPE) {
    addrinfo hints, *serverInfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(HOST, PORT, &hints, &serverInfo) != 0) {
        cerr << "Error getting address info: " << gai_strerror(errno) << endl;
        return 1;
    }

    for (auto addr = serverInfo; addr != nullptr; addr = addr->ai_next) {
        CONN = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (CONN == -1) {
            cerr << "Error creating socket: " << strerror(errno) << endl;
            continue;
        }

        if (connect(CONN, addr->ai_addr, addr->ai_addrlen) == -1) {
            cerr << "Error connecting: " << strerror(errno) << endl;
            close(CONN);
            CONN = -1;
            continue;
        }

        // If we reached here, the CONN was successful
        cout << "Connection successful." << endl;
        break;
    }

    freeaddrinfo(serverInfo);

    if (CONN == -1) {
        return -1;
    }
    return 0;
}

int Client::setupConn(const IpAddress& ipAddress, const char* CONNTYPE) {
    return setupConn(ipAddress.host.c_str(), to_string(ipAddress.port).c_str(),
                     CONNTYPE);
}

ssize_t Client::send_all_bytes(const char* buffer, size_t length, int flags,
                               int num_retries) {
    size_t total_sent = 0;
    int retries_used = 0;
    while (total_sent < length) {
        ssize_t bytes_sent =
            send(CONN, buffer + total_sent, length - total_sent, flags);

        if (bytes_sent == -1) {
            // No bytes were sent
            if (num_retries == -1 || retries_used < num_retries) {
                sleep(5);
                retries_used++;
                continue;
            }
            cerr << "Failed to send all bytes. " << total_sent << "/" << length
                 << " bytes sent after " << retries_used << " retries." << endl;
            return -1;
        }

        total_sent += bytes_sent;
    }
    return total_sent;
}

int Client::sendMsg(const string& data, int num_retries) {

    // Send message length first
    uint32_t data_size = htonl(data.size()); // Convert from host byte order to network byte order
    if (send_all_bytes(reinterpret_cast<char*>(&data_size), sizeof(data_size), 0, num_retries) == -1) {
        cerr << "Failed to send message length" << endl;
        return 1;
    }

    // Send message data
    if (send_all_bytes(data.c_str(), data.size(), 0, num_retries) == -1) {
        cerr << "Failed to send message data" << endl;
        return 1;
    }

    cout << "Client successfully sent message" << endl;

    char buffer[1024];
    ssize_t mLen = recv(CONN, buffer, sizeof(buffer), 0);
    if (mLen < 0) {
        cerr << "Error reading: " << strerror(errno) << endl;
        close(CONN);
        return 1;
    }
    buffer[mLen] = '\0';

    /*
     * If the request received contains the keyword "get", which is used to
     * represent a file transfer request, the client will proceed to provide the
     * file.
     */
    // Using `std::istringstream` instead of strtok
    std::istringstream iss(buffer);
    std::string command, filename;
    iss >> command >> filename; // Extract command and filename from message

    // process file descriptor
    if (command == "get") {
        char port[FTP_BUFFER_SIZE], buffer[FTP_BUFFER_SIZE],
            char_num_blks[FTP_BUFFER_SIZE], char_num_last_blk[FTP_BUFFER_SIZE];
        int datasock, lSize, num_blks, num_last_blk, i;
        FILE* fp;
        cout << "FTP: Filename given is: " << filename << endl;

        if (!isFileWithinDataDirectory(filename)) {
            cerr << "FTP: Requested file is not within the data directory"
                 << endl;
            send(CONN, "0", FTP_BUFFER_SIZE, 0);
            close(CONN);
            return 1;
        }

        int data_port = get_available_port();
        if (data_port == -1) {
            cerr << "FTP: No available ports" << endl;
            send(CONN, "0", FTP_BUFFER_SIZE, 0);
            close(CONN);
            return 1;
        }
        cout << "FTP: Data port is: " << data_port << endl;
        sprintf(port, "%d", data_port);
        datasock = FTP_create_socket_server(
            data_port); // creating socket for data connection
        send(CONN, port, FTP_BUFFER_SIZE, 0); // sending port no. to client
        datasock = FTP_accept_conn(datasock); // accepting connnection by client
        if ((fp = fopen(resolveDataFile(filename).c_str(), "r")) != NULL) {
            // size of file
            send(CONN, "nxt", FTP_BUFFER_SIZE, 0);
            fseek(fp, 0, SEEK_END);
            lSize = ftell(fp);
            rewind(fp);
            num_blks = lSize / FTP_BUFFER_SIZE;
            num_last_blk = lSize % FTP_BUFFER_SIZE;
            sprintf(char_num_blks, "%d", num_blks);
            send(CONN, char_num_blks, FTP_BUFFER_SIZE, 0);

            for (i = 0; i < num_blks; i++) {
                fread(buffer, sizeof(char), FTP_BUFFER_SIZE, fp);
                send(datasock, buffer, FTP_BUFFER_SIZE, 0);
            }
            sprintf(char_num_last_blk, "%d", num_last_blk);
            send(CONN, char_num_last_blk, FTP_BUFFER_SIZE, 0);
            if (num_last_blk > 0) {
                fread(buffer, sizeof(char), num_last_blk, fp);
                send(datasock, buffer, FTP_BUFFER_SIZE, 0);
            }
            fclose(fp);
            cout << "FTP: File upload done" << endl;
        } else {
            send(CONN, "0", FTP_BUFFER_SIZE, 0);
        }
    }

    close(CONN);
    return 0;
}
