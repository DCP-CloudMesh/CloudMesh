import torch
from networks import SimpleCNN
import pickle


def load_model(model_path='output/modelStateDict.bin'):
    # Load the model directly from the binary file
    model = SimpleCNN()

    # Read the binary file and unpickle it
    with open(model_path, 'rb') as file:
        unpickled_model = pickle.loads(file.read())

    model.load_state_dict(unpickled_model)
    return model


def main():
    model = load_model()
    print(model)


if __name__ == '__main__':
    main()
