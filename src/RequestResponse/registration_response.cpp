#include "../../include/RequestResponse/registration_response.h"

using namespace std;

RegistrationResponse::RegistrationResponse() : Payload(Type::REGISTRATION_RESPONSE) {}
RegistrationResponse::RegistrationResponse(const IpAddress& callerAddr)
    : Payload(Type::REGISTRATION_RESPONSE), callerAddr{callerAddr} {}

IpAddress RegistrationResponse::getCallerPublicIpAddress() const {
    return callerAddr;
}

google::protobuf::Message* RegistrationResponse::serializeToProto() const {
    payload::RegistrationResponse* proto = new payload::RegistrationResponse();
    utility::IpAddress* protoIp = new utility::IpAddress();
    protoIp->set_ip(callerAddr.host);
    protoIp->set_port(callerAddr.port);
    proto->set_allocated_calleraddr(protoIp);

    return proto;
}

void RegistrationResponse::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {
    const payload::RegistrationResponse& registrationResponseProto =
        dynamic_cast<const payload::RegistrationResponse&>(protoMessage);
    const utility::IpAddress& protoIp = registrationResponseProto.calleraddr();
    callerAddr.host = protoIp.ip();
    callerAddr.port = protoIp.port();
}
