# Peers
Classes to hold the provider and the requester information.

## Peer
The generic class to hold both the provider and the requester information. This peer can set up and initialize a server.

## Provider
The provider can do the following -
1. Register with the bootstrap node
2. Listen for incoming connections
   1. Accept the connection
   2. Receive the data
   3. Process the workload
   4. If the provider is a leader
      1. Aggregate the results from all the follower peers and send to the requester
   5. Else if the provider is a follower
      1. Send the result to the leader

## Requester
The requester can do the following -
1. Connect to the bootstrap node
2. Send the subtasks to the provider 
   1. Divide up a task into subtasks
   2. Send the subtasks to the workers
3. Get the results
   1. Busy wait until a connection is established
   2. Receive the results from the leader peer





