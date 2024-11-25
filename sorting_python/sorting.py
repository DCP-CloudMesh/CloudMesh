import zmq

# Set up the context and responder socket
base_port = int(input("Enter the base port number: "))
port_rec = base_port
port_send = base_port + 1

context = zmq.Context()
responder = context.socket(zmq.REP)
responder.bind("tcp://*:" + str(port_rec))

sender = context.socket(zmq.REQ)
sender.connect("tcp://localhost:" + str(port_send))


while True:
    # Wait for next request from client
    message = responder.recv_string()
    responder.send_string("")
    print("Unprocessed workload:", message)

    # do simple sorting work
    message = message.split(",")
    message = [int(i) for i in message]
    message.sort()
    message = [str(i) for i in message]
    message = ",".join(message)
    print(f"Processed workload: {message}")

    # Send a reply back to the client
    sender.send_string(message)
