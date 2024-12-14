#ifndef _ZMQ_RECEIVER_H_
#define _ZMQ_RECEIVER_H_

#include "../utility.h"
#include <string>
#include <zmq.hpp>

class ZMQReceiver {
    unsigned int port;
    zmq::context_t context;
    zmq::socket_t socket;

  public:
    ZMQReceiver();
    std::string receive();
    unsigned int getPort() const;
};

#endif // _ZMQ_RECEIVER_H_
