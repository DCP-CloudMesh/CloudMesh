#include "../../include/RequestResponse/acknowledgement.h"

using namespace std;
using namespace nlohmann;

Acknowledgement::Acknowledgement() : Payload(Type::ACKNOWLEDGEMENT) {}

string Acknowledgement::serialize() const { return json().dump(); }

void Acknowledgement::deserialize(const string& serializedData) {
    try {
        json j = json::parse(serializedData);
    } catch (json::exception& e) {
        cout << "JSON parsing error: " << e.what() << endl;
    }
}
