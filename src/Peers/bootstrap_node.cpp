#include "../../include/Peers/bootstrap_node.h"
#include "../../include/RequestResponse/discovery_request.h"
#include "../../include/RequestResponse/discovery_response.h"
#include "../../include/RequestResponse/message.h"
#include "../../include/RequestResponse/registration.h"

using namespace std;

BootstrapNode::BootstrapNode(const char* port, string uuid) : Peer(uuid) {
    setupServer("127.0.0.1", port);
}
BootstrapNode::~BootstrapNode() {}

const char* BootstrapNode::getServerIpAddress() {
#if defined(NOLOCAL)
    return "8.tcp.ngrok.io";
#else
    return "127.0.0.1";
#endif
}

const char* BootstrapNode::getServerPort() {
#if defined(NOLOCAL)
    return "12701";
#else
    return "8080";
#endif
}

void BootstrapNode::registerPeer(const string& peerUuid,
                                 const IpAddress& peerIpAddr) {
    providerPeers[peerUuid] = peerIpAddr;
    cout << "Registered peer " << peerUuid << " (" << peerIpAddr.host << ":"
         << peerIpAddr.port << ")" << endl;
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
        if (!server->acceptConn())
            continue;

        // receive request from peer
        string serializedData = server->receiveFromConn();
        // process this request
        string replyPrefix = "Bootstrap Node (" + uuid + ") - ";
        Message msg;
        msg.deserialize(serializedData);
        string senderUuid = msg.getSenderUuid();
        IpAddress senderIpAddr = msg.getSenderIpAddr();
        shared_ptr<Payload> payload = msg.getPayload();

        switch (payload->getType()) {
        case Payload::Type::REGISTRATION: {
            server->replyToConn(replyPrefix + "received registration request");
            registerPeer(senderUuid, senderIpAddr);
            server->replyToConn("\nRegistration successful");
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
            client->setupConn(senderIpAddr.host.c_str(),
                              to_string(senderIpAddr.port).c_str(), "tcp");
            shared_ptr<Payload> payload =
                make_shared<DiscoveryResponse>(providers);
            Message response(uuid, IpAddress(host, port), payload);
            client->sendMsg(response.serialize().c_str());
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
