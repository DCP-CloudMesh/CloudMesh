import os
import matplotlib.pyplot as plt
import numpy as np

np.random.seed(0)


def generate_temp_glob(folder):
    files = os.listdir(folder)
    files = [file for file in files if file.endswith(".jpg")]
    files = np.array(files)

    # split into three randomly but equally distributed arrays
    np.random.shuffle(files)
    one_third, two_third = int(len(files) / 3), int(2 * len(files) / 3)
    first, second, third = (
        files[:one_third],
        files[one_third:two_third],
        files[two_third:],
    )

    return first, second, third


def save_as_txt(files, filename):
    with open(filename, "w") as f:
        for file in files:
            f.write(file + "\n")


if __name__ == "__main__":
    first, second, third = generate_temp_glob("CIFAR10/train")
    save_as_txt(first, "ml/train_first.txt")
    save_as_txt(second, "ml/train_second.txt")
    save_as_txt(third, "ml/train_third.txt")

    files = os.listdir("CIFAR10/test")
    files = [file for file in files if file.endswith(".jpg")]
    save_as_txt(files, "ml/test.txt")
