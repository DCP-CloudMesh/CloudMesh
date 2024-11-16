#ifndef _REGISTRATION_
#define _REGISTRATION_

#include "payload.h"
#include <string>

class Registration : public Payload {
  public:
    Registration();

    std::string serialize() const override;
    void deserialize(const std::string& serializedData) override;
};

#endif // _REGISTRATION_
