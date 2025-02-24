#ifndef _SERVER_H
#define _SERVER_H

#include "../utility.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

class Server {
    const char* HOST;
    const char* PORT;
    const char* CONNTYPE;
    IpAddress publicIP;
    int server;     // stores the current running server id
    int activeConn; // stores the current active connection id

    /*
    Receives all bytes into a buffer with resilience to partial data sends.
    */
    ssize_t recv_all_bytes(char* buffer, size_t length, int flags, int num_retries = 0);

  public:
    Server(const char* host, const char* port, const char* type);
    ~Server();
    void setupServer();                                         // prepare server for connection
    bool acceptConn();                                          // blocking
    int receiveFromConn(std::string& msg, int num_retries = 0); // process the active conn
    void replyToConn(std::string message);                      // reply to the active conn
    void getFileFTP(std::string filename);                      // retrieve remote file
    void closeConn();                                           // close the active conn
};

#endif // _SERVER_H
