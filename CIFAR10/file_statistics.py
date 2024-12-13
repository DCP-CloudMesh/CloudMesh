import os
import matplotlib.pyplot as plt


def file_statistics(folder):
    print("Folder:", folder)
    files = os.listdir(folder)
    classes_count = dict()

    for file in files:
        class_name = file.split("_")[0]
        if class_name in classes_count:
            classes_count[class_name] += 1
        else:
            classes_count[class_name] = 1

    # plot the distribution of classes
    fig, ax = plt.subplots()
    ax.bar(classes_count.keys(), classes_count.values(), color="skyblue")
    ax.set_xlabel("Classes")
    ax.set_ylabel("Count")
    ax.set_title("Class Distribution in " + folder)
    ax.set_xticks(range(len(classes_count)))
    ax.set_xticklabels(classes_count.keys(), rotation=45, ha="right")
    ax.grid(axis="y")
    plt.tight_layout()
    plt.savefig(folder + "_class_distribution.png")


if __name__ == "__main__":
    file_statistics("train")
    file_statistics("test")
