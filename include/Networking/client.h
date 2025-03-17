#ifndef _CLIENT_H
#define _CLIENT_H

#include "../utility.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

class Client {
    int CONN;

    /*
    Sends all bytes from a buffer with resilience to partial sends with the
    option of num_retries (=-1 for blocking).
    */
    ssize_t sendAllBytes(const char* buffer, size_t length, int flags,
                           int num_retries = 0);
    void closeSocket();

  public:
    Client();
    ~Client();
    int setupConn(const IpAddress& ipAddress, const char* CONNTYPE);
    int sendMsg(const std::string& data, int num_retries = 0);
};

#endif // _CLIENT_H
