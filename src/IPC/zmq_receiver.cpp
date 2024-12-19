#include "../../include/IPC/zmq_receiver.h"

ZMQReceiver::ZMQReceiver()
    : context(1), socket(context, zmq::socket_type::rep) {
    port = get_available_port();
    address = "tcp://*:" + std::to_string(port);
    socket.bind(address);
}

std::string ZMQReceiver::getAddress() { return address; }

std::string ZMQReceiver::receive() {
    zmq::message_t zmq_msg;
    socket.recv(zmq_msg, zmq::recv_flags::none);

    // response
    // zmq::message_t reply(zmq_msg.size());
    // memcpy(reply.data(), zmq_msg.data(), zmq_msg.size());
    // socket.send(reply, zmq::send_flags::none);

    return zmq_msg.to_string();
}
