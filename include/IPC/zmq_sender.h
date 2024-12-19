#ifndef _ZMQ_SENDER_H_
#define _ZMQ_SENDER_H_

#include "../utility.h"
#include <string>
#include <zmq.hpp>

class ZMQSender {
    unsigned int port;
    zmq::context_t context;
    zmq::socket_t socket;
    std::string address;

  public:
    ZMQSender();
    void send(const std::string& message);
    std::string getAddress();
};

#endif // _ZMQ_SENDER_H_
