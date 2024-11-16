#ifndef _DISCOVERY_REQUEST_
#define _DISCOVERY_REQUEST_

#include "payload.h"
#include <string>

class DiscoveryRequest : public Payload {
    unsigned int peersRequested;

  public:
    DiscoveryRequest();
    DiscoveryRequest(const unsigned int peersRequested);

    unsigned int getPeersRequested() const;
    std::string serialize() const override;
    void deserialize(const std::string& serializedData) override;
};

#endif // _DISCOVERY_REQUEST_
