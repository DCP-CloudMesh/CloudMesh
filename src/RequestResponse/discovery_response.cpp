#include "../../include/RequestResponse/discovery_response.h"

using namespace std;

DiscoveryResponse::DiscoveryResponse() : Payload(Type::DISCOVERY_RESPONSE) {}
DiscoveryResponse::DiscoveryResponse(const AddressTable& availablePeers)
    : Payload(Type::DISCOVERY_RESPONSE), availablePeers{availablePeers} {}

AddressTable DiscoveryResponse::getAvailablePeers() const {
    return availablePeers;
}

google::protobuf::Message* DiscoveryResponse::serializeToProto() const {
    payload::DiscoveryResponse* proto = new payload::DiscoveryResponse();
    utility::AddressTable* atProto = serializeAddressTable(availablePeers);
    proto->set_allocated_availablepeers(atProto);

    return proto;
}

void DiscoveryResponse::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {

    const payload::DiscoveryResponse& discoveryResponseProto =
        dynamic_cast<const payload::DiscoveryResponse&>(protoMessage);
    const utility::AddressTable& addressTableProto =
        discoveryResponseProto.availablepeers();

    availablePeers.clear();
    availablePeers = deserializeAddressTable(addressTableProto);
}
