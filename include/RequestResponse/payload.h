#ifndef _PAYLOAD_
#define _PAYLOAD_

#include <iostream>
#include <string>

#include "proto/payload.pb.h"

// Abstract payload class
class Payload {
  public:
    enum Type {
        UNKNOWN,
        ACKNOWLEDGEMENT,
        REGISTRATION,
        REGISTRATION_RESPONSE,
        DISCOVERY_REQUEST,
        DISCOVERY_RESPONSE,
        TASK_REQUEST,
        TASK_RESPONSE,
        MODEL_STATE_DICT_PARAMS,
    };

    Payload(Type type);
    virtual ~Payload();

    Type getType() const;
    virtual google::protobuf::Message* serializeToProto() const = 0;
    virtual void
    deserializeFromProto(const google::protobuf::Message& protoMessage) = 0;

  protected:
    Type type;
};

#endif // _PAYLOAD_
