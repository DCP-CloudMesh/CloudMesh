#ifndef _REGISTRATION_RESPONSE_
#define _REGISTRATION_RESPONSE_

#include "payload.h"
#include "../utility.h"
#include <string>

class RegistrationResponse : public Payload {
    IpAddress addr;

  public:
    RegistrationResponse();
    RegistrationResponse(const IpAddress& addr);

    IpAddress getPublicIpAddress() const;
    google::protobuf::Message* serializeToProto() const override;
    void deserializeFromProto(
        const google::protobuf::Message& protoMessage) override;
};

#endif // _REGISTRATION_RESPONSE_
