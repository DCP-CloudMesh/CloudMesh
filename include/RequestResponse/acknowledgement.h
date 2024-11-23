#ifndef _ACKNOWLEDGEMENT_
#define _ACKNOWLEDGEMENT_

#include "../utility.h"
#include "payload.h"
#include <string>

class Acknowledgement : public Payload {
  public:
    Acknowledgement();

    google::protobuf::Message* serializeToProto() const override;
    void deserializeFromProto(
        const google::protobuf::Message& protoMessage) override;
};

#endif // _ACKNOWLEDGEMENT_
