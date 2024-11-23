#include "../../include/RequestResponse/discovery_response.h"

using namespace std;
using namespace nlohmann;

DiscoveryResponse::DiscoveryResponse() : Payload(Type::DISCOVERY_RESPONSE) {}
DiscoveryResponse::DiscoveryResponse(const AddressTable& availablePeers)
    : Payload(Type::DISCOVERY_RESPONSE), availablePeers{availablePeers} {}

AddressTable DiscoveryResponse::getAvailablePeers() const {
    return availablePeers;
}

string DiscoveryResponse::serialize() const {
    json j;
    j["availablePeers"] = serializeAddressTable(availablePeers);
    return j.dump();
}

void DiscoveryResponse::deserialize(const string& serializedData) {
    try {
        json j = json::parse(serializedData);
        availablePeers =
            deserializeAddressTable(j["availablePeers"].get<string>());
    } catch (json::exception& e) {
        cout << "JSON parsing error: " << e.what() << endl;
    }
}
