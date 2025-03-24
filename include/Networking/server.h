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
    IpAddress publicIp;
    const char* CONNTYPE;
    int server = -1;     // stores the current running server id
    int activeConn = -1; // stores the current active connection id

    /*
    Receives all bytes into a buffer with resilience to partial data sends.
    */
    ssize_t recvAllBytes(char* buffer, size_t length, int flags, int num_retries = 0);
    void closeSocket();

  public:
    Server(const IpAddress& addr, const char* type);
    ~Server();
    void setupServer();                                         // prepare server for connection
    bool acceptConn();                                          // blocking
    bool acceptConn(IpAddress& clientAddr);                     // blocking
    int receiveFromConn(std::string& msg, int num_retries = 0); // process the active conn
    void replyToConn(std::string message);                      // reply to the active conn
    void
    getFileIntoDirFTP(std::string filename,
               std::string directory); // retrieve remote file into directory
    void closeConn();                                           // close the active conn
};

#endif // _SERVER_H
