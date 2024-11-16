#include <iostream>
#include <nlohmann/json.hpp>

#include "../../include/RequestResponse/acknowledgement.h"
#include "../../include/RequestResponse/discovery_request.h"
#include "../../include/RequestResponse/discovery_response.h"
#include "../../include/RequestResponse/message.h"
#include "../../include/RequestResponse/registration.h"
#include "../../include/RequestResponse/task_request.h"
#include "../../include/RequestResponse/task_response.h"
#include "../../include/utility.h"

using namespace std;
using namespace nlohmann;

Message::Message() : payload{nullptr} { uuid = uuid::generate_uuid_v4(); }

Message::Message(const string& senderUuid, const IpAddress& senderIpAddr,
                 shared_ptr<Payload> payload)
    : senderUuid{senderUuid}, senderIpAddr{senderIpAddr}, payload{payload} {
    uuid = uuid::generate_uuid_v4();
}

Message::~Message() {}

string Message::getUuid() const { return uuid; }

string Message::getSenderUuid() const { return senderUuid; }

IpAddress Message::getSenderIpAddr() const { return senderIpAddr; }

shared_ptr<Payload> Message::getPayload() const { return payload; }

void Message::setPayload(std::shared_ptr<Payload> payload) {
    this->payload = payload;
}

void Message::initializePayload(const string& payloadTypeStr) {
    if (payloadTypeStr == "ACKNOWLEDGEMENT") {
        payload = make_shared<Acknowledgement>();
    } else if (payloadTypeStr == "REGISTRATION") {
        payload = make_shared<Registration>();
    } else if (payloadTypeStr == "DISCOVERY_REQUEST") {
        payload = make_shared<DiscoveryRequest>();
    } else if (payloadTypeStr == "DISCOVERY_RESPONSE") {
        payload = make_shared<DiscoveryResponse>();
    } else if (payloadTypeStr == "TASK_REQUEST") {
        payload = make_shared<TaskRequest>();
    } else if (payloadTypeStr == "TASK_RESPONSE") {
        payload = make_shared<TaskResponse>();
    } else {
        cerr << "Unknown type " << payloadTypeStr << endl;
    }
}

string Message::serialize() const {
    json j;
    j["id"] = uuid;
    j["senderId"] = senderUuid;
    j["senderIpAddress"] = serializeIpAddress(senderIpAddr);
    switch (payload->getType()) {
    case Payload::Type::ACKNOWLEDGEMENT:
        j["payloadType"] = "ACKNOWLEDGEMENT";
        break;
    case Payload::Type::REGISTRATION:
        j["payloadType"] = "REGISTRATION";
        break;
    case Payload::Type::DISCOVERY_REQUEST:
        j["payloadType"] = "DISCOVERY_REQUEST";
        break;
    case Payload::Type::DISCOVERY_RESPONSE:
        j["payloadType"] = "DISCOVERY_RESPONSE";
        break;
    case Payload::Type::TASK_REQUEST:
        j["payloadType"] = "TASK_REQUEST";
        break;
    case Payload::Type::TASK_RESPONSE:
        j["payloadType"] = "TASK_RESPONSE";
        break;
    default:
        j["payloadType"] = "";
    }
    j["payload"] = payload->serialize();

    return j.dump();
}

void Message::deserialize(const string& serializedData) {
    try {
        json j = json::parse(serializedData);
        uuid = j["id"].get<string>();
        senderUuid = j["senderId"].get<string>();
        senderIpAddr = deserializeIpAddress(j["senderIpAddress"].get<string>());
        initializePayload(j["payloadType"].get<string>());
        payload->deserialize(j["payload"].get<string>());
    } catch (json::exception& e) {
        cout << "JSON parsing error: " << e.what() << endl;
    }
}
