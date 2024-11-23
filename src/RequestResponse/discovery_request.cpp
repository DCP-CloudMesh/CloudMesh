#include "../../include/RequestResponse/discovery_request.h"

using namespace std;

DiscoveryRequest::DiscoveryRequest() : Payload(Type::DISCOVERY_REQUEST) {}
DiscoveryRequest::DiscoveryRequest(const unsigned int peersRequested)
    : Payload(Type::DISCOVERY_REQUEST), peersRequested{peersRequested} {}

unsigned int DiscoveryRequest::getPeersRequested() const {
    return peersRequested;
}

google::protobuf::Message* DiscoveryRequest::serializeToProto() const {
    payload::DiscoveryRequest* proto = new payload::DiscoveryRequest();
    proto->set_peersrequested(peersRequested);
    return proto;
}

void DiscoveryRequest::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {

    const payload::DiscoveryRequest& discoveryRequestProto =
        dynamic_cast<const payload::DiscoveryRequest&>(protoMessage);
    peersRequested = discoveryRequestProto.peersrequested();
}
