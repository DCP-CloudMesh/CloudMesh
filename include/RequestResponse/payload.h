#ifndef _PAYLOAD_
#define _PAYLOAD_

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

// Abstract payload class
class Payload {
  public:
    enum Type {
        UNKNOWN,
        ACKNOWLEDGEMENT,
        REGISTRATION,
        DISCOVERY_REQUEST,
        DISCOVERY_RESPONSE,
        TASK_REQUEST,
        TASK_RESPONSE
    };

    Payload(Type type);
    virtual ~Payload();

    Type getType() const;
    virtual std::string serialize() const = 0;
    virtual void deserialize(const std::string& serializedData) = 0;

  protected:
    Type type;
};

#endif // _PAYLOAD_
