from tqdm import tqdm


def train(model, device, train_loader, optimizer, criterion, epoch):
    model.train()
    running_loss = 0.0
    # Wrap your data loader with tqdm for a progress bar
    progress_bar = tqdm(
        enumerate(train_loader),
        total=len(train_loader),
        desc="Training Epoch {}".format(epoch + 1),
    )
    for batch_idx, (data, target) in progress_bar:
        data, target = data.to(device), target.to(device)
        optimizer.zero_grad()
        output = model(data)
        loss = criterion(output, target)
        loss.backward()
        optimizer.step()

        running_loss += loss.item()
        # Update the progress bar with the latest loss information
        progress_bar.set_postfix(
            loss=running_loss / (batch_idx + 1), current_batch=batch_idx, refresh=True
        )

    # At the end of the epoch, print the average loss
    print("[Epoch %d] loss: %.3f" % (epoch + 1, running_loss / len(train_loader)))


def train_distributed(model, device, train_loader, optimizer, criterion, epoch):
    model.train()
    running_loss = 0.0
    gradients = {}
    # Wrap your data loader with tqdm for a progress bar
    progress_bar = tqdm(
        enumerate(train_loader),
        total=len(train_loader),
        desc="Training Epoch {}".format(epoch + 1),
    )
    for batch_idx, (data, target) in progress_bar:
        data, target = data.to(device), target.to(device)
        optimizer.zero_grad()
        output = model(data)
        loss = criterion(output, target)
        loss.backward()
        optimizer.step()

        running_loss += loss.item()

        progress_bar.set_postfix(
            loss=running_loss / (batch_idx + 1), current_batch=batch_idx, refresh=True
        )

    for name, parameter in model.named_parameters():
        if parameter.grad is not None:
            gradients[name] = parameter.grad.clone().to("cpu")

    print("[Epoch %d] loss: %.3f" % (epoch + 1, running_loss / len(train_loader)))

    return gradients
