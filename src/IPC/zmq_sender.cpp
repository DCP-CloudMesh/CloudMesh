#include "../../include/IPC/zmq_sender.h"

ZMQSender::ZMQSender() : context(1), socket(context, zmq::socket_type::req) {
    port = get_available_port();
    address = "tcp://localhost:" + std::to_string(port);
    socket.connect(address);
}

std::string ZMQSender::getAddress() { return address; }

void ZMQSender::send(const std::string& message) {
    zmq::message_t zmq_msg(message.size());
    memcpy(zmq_msg.data(), message.data(), message.size());
    socket.send(zmq_msg, zmq::send_flags::none);

    zmq::message_t reply;
    socket.recv(reply, zmq::recv_flags::none);
}