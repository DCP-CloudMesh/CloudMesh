#include "../../include/RequestResponse/discovery_response.h"

using namespace std;

DiscoveryResponse::DiscoveryResponse() : Payload(Type::DISCOVERY_RESPONSE) {}
DiscoveryResponse::DiscoveryResponse(const IpAddress& callerAddr,
    const AddressTable& availablePeers)
    : Payload(Type::DISCOVERY_RESPONSE), callerAddr{callerAddr},
      availablePeers{availablePeers} {}

IpAddress DiscoveryResponse::getCallerPublicIpAddress() const {
    return callerAddr;
}

AddressTable DiscoveryResponse::getAvailablePeers() const {
    return availablePeers;
}

google::protobuf::Message* DiscoveryResponse::serializeToProto() const {
    payload::DiscoveryResponse* proto = new payload::DiscoveryResponse();
    utility::IpAddress* protoIp = new utility::IpAddress();
    protoIp->set_ip(callerAddr.host);
    protoIp->set_port(callerAddr.port);
    proto->set_allocated_calleraddr(protoIp);
    utility::AddressTable* atProto = serializeAddressTable(availablePeers);
    proto->set_allocated_availablepeers(atProto);

    return proto;
}

void DiscoveryResponse::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {
    const payload::DiscoveryResponse& discoveryResponseProto =
        dynamic_cast<const payload::DiscoveryResponse&>(protoMessage);

    const utility::IpAddress& protoIp = discoveryResponseProto.calleraddr();
    callerAddr.host = protoIp.ip();
    callerAddr.port = protoIp.port();

    const utility::AddressTable& addressTableProto =
        discoveryResponseProto.availablepeers();

    availablePeers.clear();
    availablePeers = deserializeAddressTable(addressTableProto);
}
