from tqdm import tqdm

import pickle
from proto import payload_pb2

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


def train_multiple_aggregation(model, device, train_loader, optimizer, criterion, epoch, agg_cycle, receiver, sender):
    """
    This function is equivalent to the train function above however the difference is that this 
    function will use ZMQSender and ZMQReceiver to send and receive model state dicts 
    """ 
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

        if batch_idx % agg_cycle == 0:
            # non compressed, non protobuf sending weights
            pickled_weights = pickle.dumps(model.state_dict())
            task_response = payload_pb2.TaskResponse()
            task_response.modelStateDict = pickled_weights
            sender.send(task_response.SerializeToString())
            
            # recieve the updated message
            payload = receiver.receive()
            agg_inp = payload_pb2.ModelStateDictParams()
            agg_inp.ParseFromString(payload)
            averaged_state_dict = pickle.loads(agg_inp.modelStateDict)

            # update current model with the averaged state dict
            model.load_state_dict(averaged_state_dict)

    # At the end of the epoch, print the average loss
    print("[Epoch %d] loss: %.3f" % (epoch + 1, running_loss / len(train_loader)))


# def train_distributed(model, device, train_loader, optimizer, criterion, epoch):
#     model.train()
#     running_loss = 0.0
#     gradients = {}
#     # Wrap your data loader with tqdm for a progress bar
#     progress_bar = tqdm(
#         enumerate(train_loader),
#         total=len(train_loader),
#         desc="Training Epoch {}".format(epoch + 1),
#     )
#     for batch_idx, (data, target) in progress_bar:
#         data, target = data.to(device), target.to(device)
#         optimizer.zero_grad()
#         output = model(data)
#         loss = criterion(output, target)
#         loss.backward()
#         optimizer.step()

#         running_loss += loss.item()

#         progress_bar.set_postfix(
#             loss=running_loss / (batch_idx + 1), current_batch=batch_idx, refresh=True
#         )

#     for name, parameter in model.named_parameters():
#         if parameter.grad is not None:
#             gradients[name] = parameter.grad.clone().to("cpu")

#     print("[Epoch %d] loss: %.3f" % (epoch + 1, running_loss / len(train_loader)))

#     return gradients
