import os
import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import datasets, transforms
import time
import zmq
import pickle

from utils import test
from dataloader import CIFAR10Dataset, get_data_loaders


def main():
    port_first = 5555
    port_second = 5557
    port_third = 5559
    port_aggregator = 5561

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

    responder_aggregator = context.socket(zmq.REP)
    responder_aggregator.setsockopt(zmq.LINGER, 0)
    responder_aggregator.bind("tcp://*:" + str(port_aggregator))
    sender_aggregator = context.socket(zmq.REQ)
    sender_aggregator.connect("tcp://localhost:" + str(port_aggregator + 1))

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

    # send the models to the aggregator
    print("Sending models to aggregator")

    for model in models:
        sender_aggregator.send(pickle.dumps(model))
        ack = sender_aggregator.recv_string()
        print(ack)

    # recieve the final model from the aggregator
    print("Receiving final model from aggregator")
    final_model = responder_aggregator.recv()
    final_model = pickle.loads(final_model)
    responder_aggregator.send_string("ACK")
    print("Final model received")
    print(final_model)

    # test the final model
    print("Testing the final model")
    transform = transforms.Compose(
        [
            transforms.Resize((32, 32)),
            transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5)),
        ]
    )
    payload = "test.txt"
    data_path = "CIFAR10/"
    data_file_names = []
    with open(os.path.join(data_path, payload)) as f:
        data_file_names = f.read().splitlines()
    test_dataset = CIFAR10Dataset(
        os.path.join(data_path, "test"), data_file_names, transform=transform
    )
    batch_size = 64
    test_loader = torch.utils.data.DataLoader(test_dataset, batch_size=batch_size)
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    final_model.to(device)
    criterion = nn.CrossEntropyLoss()
    test(final_model, device, test_loader, criterion, data_path)

    return


if __name__ == "__main__":
    main()
