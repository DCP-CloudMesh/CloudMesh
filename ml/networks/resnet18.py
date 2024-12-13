import torch
import torchvision.models as models
import torch.nn as nn


class Resnet18(nn.Module):
    def __init__(self, num_classes=10):
        super(Resnet18, self).__init__()
        self.resnet18 = models.resnet18(weights=models.ResNet18_Weights.IMAGENET1K_V1)
        self.resnet18.fc = nn.Linear(self.resnet18.fc.in_features, num_classes)

        # # freezing all layers on backbone
        # for param in self.resnet18.parameters():
        #     param.requires_grad = False

        # # only training fully connected layer and last conv layer
        # for param in self.resnet18.layer4.parameters():
        #     param.requires_grad = True
        # for param in self.resnet18.fc.parameters():
        #     param.requires_grad = True

        # printing parameters
        for name, param in self.resnet18.named_parameters():
            print(name, param.requires_grad)

    def forward(self, x):
        return self.resnet18(x)
