#ifndef _ZMQ_RECEIVER_H_
#define _ZMQ_RECEIVER_H_

#include <string>
#include <zmq.hpp>

class ZMQReceiver {
    unsigned int port;
    zmq::context_t context;
    zmq::socket_t socket;

  public:
    ZMQReceiver() = delete;
    ZMQReceiver(unsigned int port);
    std::string receive();
};

#endif // _ZMQ_RECEIVER_H_
