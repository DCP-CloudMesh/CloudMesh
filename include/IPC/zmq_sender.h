#ifndef _ZMQ_SENDER_H_
#define _ZMQ_SENDER_H_

#include <string>
#include <zmq.hpp>
#include "../utility.h"

class ZMQSender {
    unsigned int port;
    zmq::context_t context;
    zmq::socket_t socket;

  public:
    ZMQSender();
    void send(const std::string& message);
    unsigned int getPort() const;
};

#endif // _ZMQ_SENDER_H_
