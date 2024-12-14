import os
import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import datasets, transforms
import time
import zmq
import pickle


def main():
    port_first = 5555
    port_second = 5557
    port_third = 5559

    context = zmq.Context()

    responder1 = context.socket(zmq.REP)
    responder1.setsockopt(zmq.LINGER, 0)
    responder1.bind("tcp://*:" + str(port_first))
    sender1 = context.socket(zmq.REQ)
    sender1.connect("tcp://localhost:" + str(port_first + 1))

    responder2 = context.socket(zmq.REP)
    responder2.setsockopt(zmq.LINGER, 0)
    responder2.bind("tcp://*:" + str(port_second))
    sender2 = context.socket(zmq.REQ)
    sender2.connect("tcp://localhost:" + str(port_second + 1))

    responder3 = context.socket(zmq.REP)
    responder3.setsockopt(zmq.LINGER, 0)
    responder3.bind("tcp://*:" + str(port_third))
    sender3 = context.socket(zmq.REQ)
    sender3.connect("tcp://localhost:" + str(port_third + 1))

    payload = [
        "train_first.txt",
        "train_second.txt",
        "train_third.txt",
    ]

    # send the payload to the workers
    print("Sending payload to workers")
    sender1.send_string(payload[0])
    sender2.send_string(payload[1])
    sender3.send_string(payload[2])
    _ = sender1.recv_string()
    _ = sender2.recv_string()
    _ = sender3.recv_string()

    # recieve payload with all of the trained models
    print("Receiving models from workers")
    models = []

    model = responder1.recv()
    model = pickle.loads(model)
    models.append(model)
    responder1.send_string("ACK")

    model = responder2.recv()
    model = pickle.loads(model)
    models.append(model)
    responder2.send_string("ACK")

    model = responder3.recv()
    model = pickle.loads(model)
    models.append(model)
    responder3.send_string("ACK")

    print(models)
    return


if __name__ == "__main__":
    main()
