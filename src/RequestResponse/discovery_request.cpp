#include "../../include/RequestResponse/discovery_request.h"

using namespace std;
using namespace nlohmann;

DiscoveryRequest::DiscoveryRequest() : Payload(Type::DISCOVERY_REQUEST) {}
DiscoveryRequest::DiscoveryRequest(const unsigned int peersRequested)
    : Payload(Type::DISCOVERY_REQUEST), peersRequested{peersRequested} {}

unsigned int DiscoveryRequest::getPeersRequested() const {
    return peersRequested;
}

string DiscoveryRequest::serialize() const {
    json j;
    j["peersRequested"] = peersRequested;
    return j.dump();
}

void DiscoveryRequest::deserialize(const string& serializedData) {
    try {
        json j = json::parse(serializedData);
        peersRequested = j["peersRequested"].get<unsigned int>();
    } catch (json::exception& e) {
        cout << "JSON parsing error: " << e.what() << endl;
    }
}
