import os
import torch
from torch.utils.data import Subset, Dataset
from torchvision.io import read_image
from torchvision import datasets, transforms


class CIFAR10Dataset(Dataset):
    def __init__(self, root, transform=None):
        self.root = root
        self.transform = transform
        self.classes = (
            "airplane",
            "automobile",
            "bird",
            "cat",
            "deer",
            "dog",
            "frog",
            "horse",
            "ship",
            "truck",
        )
        self.class_to_idx = {cls_name: i for i, cls_name in enumerate(self.classes)}
        self.samples = [
            s for s in os.listdir(root) if os.path.isfile(os.path.join(root, s))
        ]

    def __len__(self):
        return len(self.samples)

    def __getitem__(self, idx):
        img_name = self.samples[idx]
        label_name = img_name.split("_")[0]
        label = self.class_to_idx[label_name]
        img_path = os.path.join(self.root, img_name)
        image = read_image(img_path).float() / 255.0
        if self.transform:
            image = self.transform(image)
        return image, label
