#include "../../include/RequestResponse/acknowledgement.h"

using namespace std;

Acknowledgement::Acknowledgement() : Payload(Type::ACKNOWLEDGEMENT) {}

google::protobuf::Message* Acknowledgement::serializeToProto() const {
    return new payload::Acknowledgement();
}

void Acknowledgement::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {

    dynamic_cast<const payload::Acknowledgement&>(protoMessage);
}
