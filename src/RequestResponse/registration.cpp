#include "../../include/RequestResponse/registration.h"

using namespace std;

Registration::Registration() : Payload(Type::REGISTRATION) {}

google::protobuf::Message* Registration::serializeToProto() const {
    return new payload::Registration();
}

void Registration::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {
    dynamic_cast<const payload::Registration&>(protoMessage);
}
