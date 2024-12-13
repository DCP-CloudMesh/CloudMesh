from dataloader import get_data_loaders, CIFAR10Dataset
import matplotlib.pyplot as plt
import os
from torchvision import transforms
import numpy as np


def simple_data_loading():
    # transformations
    transform = transforms.Compose(
        [
            transforms.Resize((32, 32)),
            transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5)),
        ]
    )

    # loading data
    data_path = "CIFAR10/"
    if not os.path.exists(os.path.join(data_path, "output")):
        os.mkdir(os.path.join(data_path, "output"))

    # train and test datasets
    train_dataset = CIFAR10Dataset(
        os.path.join(data_path, "train"), transform=transform
    )
    test_dataset = CIFAR10Dataset(os.path.join(data_path, "test"), transform=transform)

    # get data loaders
    train_loader, val_loader, test_loader = get_data_loaders(
        train_dataset, test_dataset, batch_size=32
    )

    # visualize four batches of training data
    fig, ax = plt.subplots(4, 8, figsize=(15, 8))
    for i, (images, labels) in enumerate(train_loader):
        if i == 4:
            break
        for j, (image, label) in enumerate(zip(images, labels)):
            if j >= 8:
                break
            # Denormalize the image
            img = image.permute(1, 2, 0).numpy()
            img = img * 0.5 + 0.5
            img = np.clip(img, 0, 1)
            ax[i, j].imshow(img)
            ax[i, j].set_title(train_dataset.classes[label.item()])
            ax[i, j].axis("off")
    plt.tight_layout()
    plt.savefig(os.path.join(data_path, "output", "train_data.png"))
    # plt.show()


if __name__ == "__main__":
    simple_data_loading()
