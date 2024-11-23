#ifndef _PEER_H
#define _PEER_H

#include <string>

#include "../Networking/client.h"
#include "../Networking/server.h"
#include "../utility.h"

class Peer {
  protected:
    const char* host;
    const char* port;
    std::string uuid;

    Server* server;
    Client* client;
    // maps uuid to ip address
    AddressTable providerPeers;
    IpAddress leaderIP;

  public:
    Peer();
    Peer(const std::string& uuid);
    void setupServer(const char* host, const char* port);
    virtual ~Peer();
};

#endif // _PEER_H
