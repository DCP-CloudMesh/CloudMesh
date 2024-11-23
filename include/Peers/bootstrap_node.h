#ifndef _BOOTSTRAP_NODE_H
#define _BOOTSTRAP_NODE_H

#include "../utility.h"
#include <string>

#include "peer.h"

class BootstrapNode : public Peer {
  public:
    BootstrapNode(const char*, std::string);
    // ----------------- FIX LATER -----------------
    BootstrapNode() {}
    ~BootstrapNode();

    static const char* getServerIpAddress();
    static const char* getServerPort();

    void registerPeer(const std::string& peerUuid, const IpAddress& peerIpAddr);
    AddressTable discoverPeers(const std::string& peerUuid,
                               const unsigned int peersRequested);
    void listen();
};

#endif // _BOOTSTRAP_NODE_H
