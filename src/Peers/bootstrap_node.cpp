#include "../../include/Peers/bootstrap_node.h"
#include "../../include/RequestResponse/discovery_request.h"
#include "../../include/RequestResponse/discovery_response.h"
#include "../../include/RequestResponse/message.h"
#include "../../include/RequestResponse/registration.h"
#include "../../include/RequestResponse/registration_response.h"

using namespace std;

BootstrapNode::BootstrapNode(string uuid) : Peer(uuid) {
    setupServer(getServerIpAddr());
}
BootstrapNode::~BootstrapNode() {}

IpAddress BootstrapNode::getServerIpAddr() {
#if defined(NOLOCAL)
    return IpAddress("25.8.164.169", 8080);
#else
    return IpAddress("127.0.0.1", 8080);
#endif
}

void BootstrapNode::registerPeer(const string& peerUuid,
                                 const IpAddress& peerIpAddr) {
    providerPeers[peerUuid] = peerIpAddr;
    cout << "Registered peer " << peerUuid << " (" << peerIpAddr << ")" << endl;
}

AddressTable BootstrapNode::discoverPeers(const string& peerUuid,
                                          const unsigned int peersRequested) {
    AddressTable providers;
    for (auto& it : providerPeers) {
        if (providers.size() == peersRequested)
            break;
        if (it.first != peerUuid) {
            providers[it.first] = it.second;
        }
    }
    return providers;
}

void BootstrapNode::listen() {
    while (true) {
        cout << "Waiting for peer to connect..." << endl;
        IpAddress senderClientIpAddr;

        if (!server->acceptConn(senderClientIpAddr)) {
            continue;
        }

        // receive request from peer
        string serializedData;
        if (server->receiveFromConn(serializedData) == 1) {
            server->closeConn();
            continue;
        }

        // Deserialize request
        Message msg;
        msg.deserialize(serializedData);

        IpAddress senderServerIpAddr = senderClientIpAddr;
        // Use server port specified in message (different from client port)
        senderServerIpAddr.port = msg.getSenderIpAddr().port;

        string senderUuid = msg.getSenderUuid();
        shared_ptr<Payload> payload = msg.getPayload();

        string replyPrefix = "Bootstrap Node (" + uuid + ") - ";
        switch (payload->getType()) {
        case Payload::Type::REGISTRATION: {
            server->replyToConn(replyPrefix + "received registration request");
            cout << "received registration request from " << senderServerIpAddr.host << endl;
            registerPeer(senderUuid, senderServerIpAddr);
            server->replyToConn("\nRegistration successful");
            // Create response
            client->setupConn(senderServerIpAddr, "tcp");
            shared_ptr<Payload> payload =
                make_shared<RegistrationResponse>(senderServerIpAddr);
            Message response(uuid, publicIp, payload);;
            client->sendMsg(response.serialize());
            break;
        }
        case Payload::Type::DISCOVERY_REQUEST: {
            server->replyToConn(replyPrefix + "received discovery request");
            shared_ptr<DiscoveryRequest> dr =
                static_pointer_cast<DiscoveryRequest>(payload);
            unsigned int numPeersRequested = dr->getPeersRequested();
            AddressTable providers =
                discoverPeers(senderUuid, numPeersRequested);
            server->replyToConn("\nFound " + to_string(providers.size()) +
                                " provider(s)");
            // Create response
            client->setupConn(senderServerIpAddr, "tcp");
            shared_ptr<Payload> payload =
                make_shared<DiscoveryResponse>(senderServerIpAddr, providers);
            Message response(uuid, publicIp, payload);
            client->sendMsg(response.serialize());
            break;
        }
        default:
            cerr << "Received unsupported message type" << endl;
            server->replyToConn(replyPrefix +
                                "received unsupported message type");
        }
        server->closeConn();
    }
}
