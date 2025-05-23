#include "../../include/IPC/zmq_receiver.h"

ZMQReceiver::ZMQReceiver()
    : context(1), socket(context, zmq::socket_type::rep) {
    port = get_available_port();
    address = "tcp://*:" + std::to_string(port);
    socket.bind(address);
}

unsigned int ZMQReceiver::getPort() { return port; }
std::string ZMQReceiver::getAddress() { return address; }

std::string ZMQReceiver::receive() {
    zmq::message_t zmq_msg;
    socket.recv(zmq_msg, zmq::recv_flags::none);

    // response
    std::string ack = "ACK";
    zmq::message_t reply(ack.size());
    memcpy(reply.data(), ack.data(), ack.size());
    socket.send(reply, zmq::send_flags::none);

    return zmq_msg.to_string();
}
