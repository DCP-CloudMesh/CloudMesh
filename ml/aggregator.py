# This is the main script for training a SimpleCNN model on CIFAR10 dataset.
import os
import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import datasets, transforms
import time
import pickle
import zmq
import argparse

from networks import SimpleCNN
from dataloader import CIFAR10Dataset, get_data_loaders
from utils import train, val, test, network

from proto import payload_pb2, utility_pb2


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
    parser = argparse.ArgumentParser()
    parser.add_argument('--port_rec', type=int, required=True,
                        help='Aggregator sender port number')
    parser.add_argument('--port_send', type=int,
                        required=True, help='Aggregator receiver port number')
    parser.add_argument('--num_peers', type=int, required=True,
                        help='Number of peers to wait for')
    args = parser.parse_args()
    port_rec = int(args.port_rec)
    port_send = int(args.port_send)
    num_peers = int(args.num_peers)

    context = zmq.Context()
    receiver = network.ZMQReciever(context, port_rec)
    sender = network.ZMQSender(context, port_send)

    numCompletePeers = 0
    # completeStateDicts = []
    final_state_dict = None

    # recieve the models from fake_peer.py
    while True:
        print("Waiting for models...")
        # state_dicts = completeStateDicts.copy()
        state_dicts = []
        for _ in range(num_peers - numCompletePeers):
            payload = receiver.receive()
            agg_inp = payload_pb2.ModelStateDictParams()
            agg_inp.ParseFromString(payload)
            agg_inp_model = pickle.loads(agg_inp.modelStateDict)
            if agg_inp.trainingIsComplete:
                numCompletePeers += 1
                # completeStateDicts.append(agg_inp_model)
            state_dicts.append(agg_inp_model)
            time.sleep(5)

        # average the models
        print("Averaging models...")
        avg_state_dict = nn_aggregator(state_dicts)

        if numCompletePeers > 0:
            final_state_dict = avg_state_dict
            break

        # send the averaged model back to fake_peer.py
        print("Sending averaged model...")
        tr = payload_pb2.TaskResponse()
        tr.modelStateDict = pickle.dumps(avg_state_dict)
        tr.trainingIsComplete = False
        sender.send(tr.SerializeToString())

    # send final response
    print("Sending final response...")
    pickled_weights = pickle.dumps(final_state_dict)
    task_response = payload_pb2.TaskResponse()
    task_response.modelStateDict = pickled_weights
    task_response.trainingIsComplete = True
    sender.send(task_response.SerializeToString())

    return


if __name__ == "__main__":
    main()
