#include "../../include/RequestResponse/registration.h"

using namespace std;

Registration::Registration() : Payload(Type::REGISTRATION) {}

string Registration::serialize() const { return ""; }

void Registration::deserialize(const string& serializedData) {}
