#include "../../include/Peers/peer.h"

#include "../../include/utility.h"

using namespace std;

Peer::Peer() : server{nullptr}, client{nullptr} {
    // initialize client
    client = new Client();

    // instantiate uuid
    uuid = uuid::generate_uuid_v4();
}

Peer::Peer(const string& uuid)
    : uuid{uuid}, server{nullptr}, client{nullptr} {
    // initialize client
    client = new Client();
}

void Peer::setPublicIp(const IpAddress& addr) {
    publicIp = addr;
}

void Peer::setupServer(const IpAddress& addr) {
    setPublicIp(addr);
    const char* type = "tcp";

    // Tear down existing server
    if (server != nullptr) {
        delete server;
    }

    // Initialize server
    server = new Server(addr, type);
    server->setupServer();
}

Peer::~Peer() {
    delete client;
    delete server;
    client = nullptr;
    server = nullptr;
}
