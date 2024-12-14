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


def main():
    # Set up the context and responder socket
    port_rec = int(input("Enter the ZMQ sender port number: "))
    port_send = int(input("Enter the ZMQ receiver port number: "))

    context = zmq.Context()
    responder = context.socket(zmq.REP)
    responder.setsockopt(zmq.LINGER, 0)
    responder.bind("tcp://*:" + str(port_rec))

    sender = context.socket(zmq.REQ)
    sender.setsockopt(zmq.LINGER, 0)
    sender.connect("tcp://localhost:" + str(port_send))

    # Hyperparameters (can use CLI)
    batch_size = 64
    learning_rate = 0.001
    epochs = 1  # for now
    device = torch.device(
        "mps" if torch.backends.mps.is_available() else "cpu")
    print(f"Using device: {device}")

    # recieve a payload with the data_file_names
    payload = responder.recv_string()
    responder.send_string("")
    data_file_names = str(payload)
    start_time = time.time()

    # Define transforms
    transform = transforms.Compose(
        [
            transforms.Resize((32, 32)),
            transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5)),
        ]
    )

    # Create the datasets
    data_path = "CIFAR10/"
    with open(os.path.join(data_path, data_file_names)) as f:
        data_file_names = f.read().splitlines()
    if not os.path.exists(os.path.join(data_path, "output")):
        os.mkdir(os.path.join(data_path, "output"))
    train_dataset = CIFAR10Dataset(
        os.path.join(data_path, "train"), data_file_names, transform=transform
    )
    print("train_dataset length: ", len(train_dataset))

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
    for epoch in range(epochs):
        print("training")
        train(model, device, train_loader, optimizer, criterion, epoch)
        print("validating")
        val(model, device, val_loader, criterion, epoch, data_path)

    # Test the model
    test(model, device, test_loader, criterion, data_path)

    # Save the model checkpoint
    # torch.save(model.state_dict(), f"{data_path}output/model.pth")
    # print("Finished Training. Model saved as model.pth.")

    end_time = time.time()
    print("Total Time: ", end_time - start_time)
    print("Start Time: ", start_time)
    print("End Time: ", end_time)

    # non compressed, non protobuf sending weights
    pickled_weights = pickle.dumps(model.state_dict())
    sender.send(pickled_weights)
    ack = sender.recv_string()
    print(ack)

    return


if __name__ == "__main__":
    main()
