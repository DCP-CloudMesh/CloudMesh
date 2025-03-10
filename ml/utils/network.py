import zmq

# sender sends the message
class ZMQSender:
    def __init__(self, context, port_send):
        _sender = context.socket(zmq.REQ)
        _sender.setsockopt(zmq.LINGER, 0)
        _sender.connect("tcp://localhost:" + str(port_send))
        self.sender = _sender
    
    # send message over zmq needs an acknowledgement as well
    def send(self, msg):
        self.sender.send(msg)
        ack = self.sender.recv()
        return ack


# receiver receives the message
class ZMQReciever:
    def __init__(self, context, port_rec):
        _receiver = context.socket(zmq.REP)
        _receiver.setsockopt(zmq.LINGER, 0)
        _receiver.bind("tcp://*:" + str(port_rec))
        self.receiver = _receiver
    
    # receive message over zmq needs to send an acknowledgement as well
    def recv(self):
        msg = self.receiver.recv()
        self.receiver.send_string("")
        return msg