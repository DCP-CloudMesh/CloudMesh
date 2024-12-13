import torch
import matplotlib.pyplot as plt
from sklearn.metrics import precision_score, recall_score, f1_score, confusion_matrix
import seaborn as sns
import matplotlib.pyplot as plt
import json


def test(model, device, test_loader, criterion, output_path, num_classes=10):
    model.eval()
    test_loss = 0
    correct = 0
    targets = []
    preds = []

    # Confusion matrix
    conf_mat = torch.zeros(num_classes, num_classes, dtype=torch.int64)

    with torch.no_grad():
        for data, target in test_loader:
            data, target = data.to(device), target.to(device)
            output = model(data)
            test_loss += criterion(output, target).item()
            pred = output.argmax(dim=1, keepdim=False)
            correct += pred.eq(target).sum().item()

            # Collect true and predicted values for use with sklearn
            targets.extend(target.view_as(pred).cpu().numpy())
            preds.extend(pred.cpu().numpy())

            # Update confusion matrix
            for t, p in zip(target.view(-1), pred.view(-1)):
                conf_mat[t.long(), p.long()] += 1

    # Metrics calculation
    test_loss /= len(test_loader.dataset)
    accuracy = correct / len(test_loader.dataset)
    precision = precision_score(targets, preds, average="macro")
    recall = recall_score(targets, preds, average="macro")
    f1 = f1_score(targets, preds, average="macro")

    # Print results
    print(
        f"\nTest set: Average loss: {test_loss:.4f}, Accuracy: {correct}/{len(test_loader.dataset)} ({100. * accuracy:.0f}%)"
    )
    print(f"Precision: {precision:.4f}, Recall: {recall:.4f}, F1: {f1:.4f}\n")

    # Save numeric metrics
    metrics = {
        "loss": test_loss,
        "accuracy": accuracy,
        "precision": precision,
        "recall": recall,
        "f1": f1,
        "confusion_matrix": conf_mat.tolist(),
    }
    with open(f"{output_path}output/metrics_final.json", "w") as f:
        json.dump(metrics, f, indent=4)

    # Plot confusion matrix
    fig, ax = plt.subplots(figsize=(10, 10))
    sns.heatmap(
        conf_mat.numpy(),
        annot=True,
        fmt="d",
        xticklabels=[f"Class {i}" for i in range(num_classes)],
        yticklabels=[f"Class {i}" for i in range(num_classes)],
        ax=ax,
    )
    plt.ylabel("Actual")
    plt.xlabel("Predicted")
    plt.title(f"Confusion Matrix on Test Set")

    # Save confusion matrix figure
    filename = f"{output_path}output/confusion_matrix_final.png"
    plt.savefig(filename, bbox_inches="tight")
    plt.close()
