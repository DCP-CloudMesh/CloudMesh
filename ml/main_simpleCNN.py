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
from utils import train, val, test, network
from proto import payload_pb2, utility_pb2
import argparse
import warnings


def main():
    # Set up the context and receiver socket
    parser = argparse.ArgumentParser()
    parser.add_argument('--port_rec', type=int, required=True,
                        help='ZMQ sender port number')
    parser.add_argument('--port_send', type=int,
                        required=True, help='ZMQ receiver port number')
    args = parser.parse_args()
    port_rec = int(args.port_rec)
    port_send = int(args.port_send)

    context = zmq.Context()
    receiver = network.ZMQReciever(context, port_rec)
    sender = network.ZMQSender(context, port_send)

    # Hyperparameters (can use CLI)
    batch_size = 64
    learning_rate = 0.001
    # epochs = 2  # for now
    device = torch.device(
        "mps" if torch.backends.mps.is_available() else "cpu")
    print(f"Using device: {device}")

    while True:
        # recieve a payload with the data_file_names
        print('\n=== ACCEPTING TRAINING TASK REQUEST ===\n')
        payload = receiver.receive()
        training_payload = payload_pb2.TrainingData()
        training_payload.ParseFromString(payload)
        data_file_names = training_payload.training_data_index_filename
        epochs = training_payload.numEpochs
        print("data_file_names: ", data_file_names)
        # print hyperparameters
        print("batch_size: ", batch_size)
        print("learning_rate: ", learning_rate)
        print("epochs: ", epochs)

        start_time = time.time()

        # Define transforms
        transform = transforms.Compose(
            [
                transforms.Resize((32, 32)),
                transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5)),
            ]
        )

        # Create the datasets
        data_path = "CIFAR10/train"
        with open(os.path.join(data_path, data_file_names)) as f:
            data_file_names = f.read().splitlines()
        if not os.path.exists(os.path.join(data_path, "output")):
            os.mkdir(os.path.join(data_path, "output"))
        train_dataset = CIFAR10Dataset(
            data_path, data_file_names, transform=transform
        )
        print("train_dataset length: ", len(train_dataset))

        data_path = "CIFAR10/"
        data_file_names = "test.txt"
        with open(os.path.join(data_path, data_file_names)) as f:
            data_file_names = f.read().splitlines()
        test_dataset = CIFAR10Dataset(
            os.path.join(data_path, "test"), data_file_names, transform=transform
        )

        # Create data loaders
        train_loader, val_loader, test_loader = get_data_loaders(
            train_dataset, test_dataset, batch_size
        )

        # Model & loss & optimizer
        model = SimpleCNN().to(device)
        criterion = nn.CrossEntropyLoss()
        optimizer = optim.Adam(model.parameters(), lr=learning_rate)

        # Train the model
        print('\n=== TRAIN & VALIDATE MODEL ===\n')
        for epoch in range(epochs):
            train(model, device, train_loader, optimizer, criterion, epoch)
            print("\nValidating")
            val(model, device, val_loader, criterion, epoch, data_path)

            # non compressed, non protobuf sending weights
            pickled_weights = pickle.dumps(model.state_dict())
            task_response = payload_pb2.TaskResponse()
            task_response.modelStateDict = pickled_weights

            # Check if on the last epoch
            if epoch == epochs - 1:
                task_response.trainingIsComplete = True

                # Test the model
                print('\n=== TEST MODEL ===\n')
                test(model, device, test_loader, criterion, data_path)

                # Save the model checkpoint
                # torch.save(model.state_dict(), f"{data_path}output/model.pth")
                # print("Finished Training. Model saved as model.pth.")

                end_time = time.time()
                print("Total Time: ", end_time - start_time)
                print("Start Time: ", start_time)
                print("End Time: ", end_time)

                sender.send(task_response.SerializeToString())
                break

            print("Publishing model state dict")
            task_response.trainingIsComplete = False
            sender.send(task_response.SerializeToString())

            # receive the updated message
            # TODO: fix this if we ignore the very last message
            print("Waiting for averaged model state dict")
            payload = receiver.receive()
            agg_inp = payload_pb2.ModelStateDictParams()
            agg_inp.ParseFromString(payload)
            averaged_state_dict = pickle.loads(agg_inp.modelStateDict)
            print("Received averaged model state dict\n")

            # update current model with the averaged state dict
            model.load_state_dict(averaged_state_dict)

        # send final response
        # pickled_weights = pickle.dumps(model.state_dict())
        # task_response = payload_pb2.TaskResponse()
        # task_response.modelStateDict = pickled_weights
        # task_response.trainingIsComplete = True
        # sender.send(task_response.SerializeToString())


if __name__ == "__main__":
    warnings.filterwarnings("ignore")
    main()
