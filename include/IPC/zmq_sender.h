#ifndef _ZMQ_SENDER_H_
#define _ZMQ_SENDER_H_

#include <string>
#include <zmq.hpp>

class ZMQSender {
    unsigned int port;
    zmq::context_t context;
    zmq::socket_t socket;

  public:
    ZMQSender() = delete;
    ZMQSender(unsigned int port);
    void send(const std::string& message);
};

#endif // _ZMQ_SENDER_H_
