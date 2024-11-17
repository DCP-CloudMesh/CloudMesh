#ifndef _REGISTRATION_
#define _REGISTRATION_

#include "payload.h"
#include <string>

class Registration : public Payload {
  public:
    Registration();

    google::protobuf::Message* serializeToProto() const override;
    void deserializeFromProto(
        const google::protobuf::Message& protoMessage) override;
};

#endif // _REGISTRATION_
