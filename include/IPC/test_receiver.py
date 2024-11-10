import zmq

# Set up the context and responder socket
port = input("Port: ")
context = zmq.Context()
responder = context.socket(zmq.REP)
responder.bind("tcp://*:" + str(port))

while True:
    # Wait for next request from client
    message = responder.recv_string()
    print("Received request:", message)

    # Send a reply back to the client
    responder.send_string("Acknowledged")