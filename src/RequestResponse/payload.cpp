#include "../../include/RequestResponse/payload.h"

using namespace std;

Payload::Payload(Payload::Type type) : type{type} {}

Payload::~Payload() {}

Payload::Type Payload::getType() const { return type; }
