#include "../../include/IPC/zmq_sender.h"

ZMQSender::ZMQSender(unsigned int port)
    : port(port), context(1), socket(context, zmq::socket_type::req) {
    const std::string address = "tcp://localhost:" + std::to_string(port);
    socket.connect(address);
}

void ZMQSender::send(const std::string& message) {
    zmq::message_t zmq_msg(message.size());
    memcpy(zmq_msg.data(), message.data(), message.size());
    socket.send(zmq_msg, zmq::send_flags::none);
}