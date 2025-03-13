#include "../../include/RequestResponse/registration_response.h"

using namespace std;

RegistrationResponse::RegistrationResponse() : Payload(Type::REGISTRATION_RESPONSE) {}
RegistrationResponse::RegistrationResponse(const IpAddress& addr)
    : Payload(Type::REGISTRATION_RESPONSE), addr{addr} {}

IpAddress RegistrationResponse::getPublicIpAddress() const {
    return addr;
}

google::protobuf::Message* RegistrationResponse::serializeToProto() const {
    payload::RegistrationResponse* proto = new payload::RegistrationResponse();
    utility::IpAddress* protoIp = new utility::IpAddress();
    protoIp->set_ip(addr.host);
    protoIp->set_port(addr.port);
    proto->set_allocated_addr(protoIp);

    return proto;
}

void RegistrationResponse::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {
    
    const payload::RegistrationResponse& registrationResponseProto =
        dynamic_cast<const payload::RegistrationResponse&>(protoMessage);
    const utility::IpAddress& protoIp = registrationResponseProto.addr();
    addr.host = protoIp.ip();
    addr.port = protoIp.port();
}
