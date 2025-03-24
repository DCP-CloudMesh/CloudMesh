#ifndef _DISCOVERY_RESPONSE_
#define _DISCOVERY_RESPONSE_

#include "../utility.h"
#include "payload.h"
#include <string>

class DiscoveryResponse : public Payload {
    IpAddress callerAddr;
    AddressTable availablePeers;

  public:
    DiscoveryResponse();
    DiscoveryResponse(const IpAddress& callerAddr, const AddressTable& availablePeers);

    IpAddress getCallerPublicIpAddress() const;
    AddressTable getAvailablePeers() const;
    google::protobuf::Message* serializeToProto() const override;
    void deserializeFromProto(
        const google::protobuf::Message& protoMessage) override;
};

#endif // _DISCOVERY_RESPONSE_
