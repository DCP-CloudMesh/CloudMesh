# This is the main script for training a SimpleCNN model on CIFAR10 dataset.
import os
import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import datasets, transforms
import time
import pickle
import zmq

from networks import SimpleCNN
from dataloader import CIFAR10Dataset, get_data_loaders
from utils import train, val, test


def nn_aggregator(state_dicts):
    """
    This function takes in a list of models and averages their weights.
    """
    # get the state_dicts of the models
    keys = state_dicts[0].keys()

    # initialize the average state_dict and do the averaging
    avg_state_dict = {}
    for key in keys:
        tensors = [
            state_dict[key].float()
            for state_dict in state_dicts
            if key in state_dict.keys()
        ]
        avg_state_dict[key] = torch.stack(tensors).mean(dim=0)

    return avg_state_dict


def main():
    # Set up the context and responder socket
    port_rec = int(input("Enter the ZMQ sender port number: "))
    port_send = int(input("Enter the ZMQ reciever port number: "))

    context = zmq.Context()
    responder = context.socket(zmq.REP)
    responder.setsockopt(zmq.LINGER, 0)
    responder.bind("tcp://*:" + str(port_rec))

    sender = context.socket(zmq.REQ)
    sender.setsockopt(zmq.LINGER, 0)
    sender.connect("tcp://localhost:" + str(port_send))

    # recieve the models from fake_peer.py
    state_dicts = []
    for i in range(3):
        sd = responder.recv()
        sd = pickle.loads(sd)
        state_dicts.append(sd)
        responder.send_string("ACK")

    # average the models
    avg_state_dict = nn_aggregator(state_dicts)

    # send the averaged model back to fake_peer.py
    avg_model = SimpleCNN()
    avg_model.load_state_dict(avg_state_dict)
    avg_model = pickle.dumps(avg_model)

    sender.send(avg_model)
    _ = sender.recv_string()

    print("Model averaging complete.")
    return


if __name__ == "__main__":
    main()
