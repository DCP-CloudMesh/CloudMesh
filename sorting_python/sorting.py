import zmq
import sys

# Set up the context and responder socket
port_rec = int(sys.argv[1])
port_send = int(sys.argv[2])

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
