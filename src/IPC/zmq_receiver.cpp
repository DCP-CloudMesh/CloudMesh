#include "../../include/IPC/zmq_receiver.h"

ZMQReceiver::ZMQReceiver(unsigned int port)
    : port(port), context(1), socket(context, zmq::socket_type::rep) {
    const std::string address = "tcp://*:" + std::to_string(port);
    socket.bind(address);
}

std::string ZMQReceiver::receive() {
    zmq::message_t zmq_msg;
    socket.recv(zmq_msg, zmq::recv_flags::none);
    return zmq_msg.to_string();
}
