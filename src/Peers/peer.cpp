#include "../../include/Peers/peer.h"

#include "../../include/utility.h"

using namespace std;

Peer::Peer() : host{nullptr}, port{nullptr}, server{nullptr}, client{nullptr} {
    // initialize client
    client = new Client();

    // instantiate uuid
    uuid = uuid::generate_uuid_v4();
}

Peer::Peer(const string& uuid)
    : host{nullptr}, port{nullptr}, uuid{uuid}, server{nullptr},
      client{nullptr} {
    // initialize client
    client = new Client();
}

void Peer::setupServer(const char* host, const char* port) {
    this->host = host;
    this->port = port;
    const char* type = "tcp";
    // initialize server
    server = new Server(host, port, type);
    server->setupServer();
}

Peer::~Peer() {
    delete client;
    delete server;
}
