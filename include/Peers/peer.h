#ifndef _PEER_H
#define _PEER_H

#include <string>

#include "../Networking/client.h"
#include "../Networking/server.h"
#include "../utility.h"

class Peer {
  protected:
    IpAddress publicIp;
    std::string uuid;

    Server* server;
    Client* client;
    // maps uuid to ip address
    AddressTable providerPeers;
    IpAddress leaderIP;

  public:
    Peer();
    Peer(const std::string& uuid);
    void setPublicIp(const IpAddress& addr);
    void setupServer(const IpAddress& addr);
    virtual ~Peer();
};

#endif // _PEER_H
