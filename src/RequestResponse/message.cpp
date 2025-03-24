#include <iostream>

#include "../../include/RequestResponse/acknowledgement.h"
#include "../../include/RequestResponse/discovery_request.h"
#include "../../include/RequestResponse/discovery_response.h"
#include "../../include/RequestResponse/message.h"
#include "../../include/RequestResponse/registration.h"
#include "../../include/RequestResponse/registration_response.h"
#include "../../include/RequestResponse/task_request.h"
#include "../../include/RequestResponse/task_response.h"
#include "../../include/RequestResponse/model_state_dict_params.h"
#include "../../include/utility.h"

using namespace std;

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
    } else if (payloadTypeStr == "REGISTRATION_RESPONSE") {
        payload = make_shared<RegistrationResponse>();
    } else if (payloadTypeStr == "DISCOVERY_REQUEST") {
        payload = make_shared<DiscoveryRequest>();
    } else if (payloadTypeStr == "DISCOVERY_RESPONSE") {
        payload = make_shared<DiscoveryResponse>();
    } else if (payloadTypeStr == "TASK_REQUEST") {
        payload = make_shared<TaskRequest>();
    } else if (payloadTypeStr == "TASK_RESPONSE") {
        payload = make_shared<TaskResponse>();
    } else if (payloadTypeStr == "MODEL_STATE_DICT_PARAMS") {
        payload = make_shared<ModelStateDictParams>();
    } else {
        cerr << "Unknown type " << payloadTypeStr << endl;
    }
}

template <typename T> std::shared_ptr<T> Message::getPayloadAs() const {
    if (!payload) {
        return nullptr;
    }

    std::shared_ptr<T> converted = std::dynamic_pointer_cast<T>(payload);
    if (!converted) {
        throw std::runtime_error("Invalid payload type cast requested");
    }

    return converted;
}

string Message::serialize() const {
    payload::PayloadMessage messageProto;
    messageProto.set_id(uuid);
    messageProto.set_senderid(senderUuid);
    messageProto.set_allocated_senderipaddress(
        serializeIpAddressToProto(senderIpAddr));

    switch (payload->getType()) {
    case Payload::Type::ACKNOWLEDGEMENT: {
        messageProto.set_payloadtype(payload::PayloadType::ACKNOWLEDGEMENT);
        shared_ptr<Acknowledgement> acknowledgement =
            getPayloadAs<Acknowledgement>();
        auto* acknowledgementProto = static_cast<payload::Acknowledgement*>(
            acknowledgement->serializeToProto());
        messageProto.set_allocated_acknowledgement(acknowledgementProto);
        break;
    }
    case Payload::Type::REGISTRATION: {
        messageProto.set_payloadtype(payload::PayloadType::REGISTRATION);
        shared_ptr<Registration> registration = getPayloadAs<Registration>();
        payload::Registration* registrationProto =
            static_cast<payload::Registration*>(
                registration->serializeToProto());
        messageProto.set_allocated_registration(registrationProto);
        break;
    }
    case Payload::Type::REGISTRATION_RESPONSE: {
        messageProto.set_payloadtype(payload::PayloadType::REGISTRATION_RESPONSE);
        shared_ptr<RegistrationResponse> registrationResponse =
            getPayloadAs<RegistrationResponse>();
        payload::RegistrationResponse* rrProto =
            static_cast<payload::RegistrationResponse*>(
                registrationResponse->serializeToProto());
        messageProto.set_allocated_registrationresponse(rrProto);
        break;
    }
    case Payload::Type::DISCOVERY_REQUEST: {
        messageProto.set_payloadtype(payload::PayloadType::DISCOVERY_REQUEST);
        shared_ptr<DiscoveryRequest> discoveryReq =
            getPayloadAs<DiscoveryRequest>();
        auto* dvProto = static_cast<payload::DiscoveryRequest*>(
            discoveryReq->serializeToProto());
        messageProto.set_allocated_discoveryrequest(dvProto);
        break;
    }
    case Payload::Type::DISCOVERY_RESPONSE: {
        messageProto.set_payloadtype(payload::PayloadType::DISCOVERY_RESPONSE);
        shared_ptr<DiscoveryResponse> discoveryResp =
            getPayloadAs<DiscoveryResponse>();
        auto* drProto = static_cast<payload::DiscoveryResponse*>(
            discoveryResp->serializeToProto());
        messageProto.set_allocated_discoveryresponse(drProto);
        break;
    }
    case Payload::Type::TASK_REQUEST: {
        messageProto.set_payloadtype(payload::PayloadType::TASK_REQUEST);
        shared_ptr<TaskRequest> taskReq = getPayloadAs<TaskRequest>();
        auto* taskReqProto =
            static_cast<payload::TaskRequest*>(taskReq->serializeToProto());
        messageProto.set_allocated_taskrequest(taskReqProto);
        break;
    }
    case Payload::Type::TASK_RESPONSE: {
        messageProto.set_payloadtype(payload::PayloadType::TASK_RESPONSE);
        shared_ptr<TaskResponse> taskResp = getPayloadAs<TaskResponse>();
        auto* taskRespProto =
            static_cast<payload::TaskResponse*>(taskResp->serializeToProto());
        messageProto.set_allocated_taskresponse(taskRespProto);
        break;
    }
    case Payload::Type::MODEL_STATE_DICT_PARAMS: {
        messageProto.set_payloadtype(payload::PayloadType::MODEL_STATE_DICT_PARAMS);
        shared_ptr<ModelStateDictParams> modelParams = getPayloadAs<ModelStateDictParams>();
        auto* modelParamsProto =
            static_cast<payload::ModelStateDictParams*>(modelParams->serializeToProto());
        messageProto.set_allocated_modelstatedictparams(modelParamsProto);
        break;
    }
    default:
        cerr << "Unknown type" << endl;
        exit(EXIT_FAILURE);
    }

    string serialized;
    messageProto.SerializeToString(&serialized);

    return serialized;
}

void Message::deserialize(const string& serializedData) {
    payload::PayloadMessage messageProto;
    if (!messageProto.ParseFromString(serializedData)) {
        cerr << "Failed to parse Message from string" << endl;
        exit(EXIT_FAILURE);
    }

    uuid = messageProto.id();
    senderUuid = messageProto.senderid();
    senderIpAddr =
        deserializeIpAddressFromProto(messageProto.senderipaddress());

    cout << "Deserializing message with id: " << uuid << " from " << senderUuid
         << " at " << senderIpAddr << endl;

    string payloadType = payload::PayloadType_Name(messageProto.payloadtype());
    initializePayload(payloadType);

    // deserialize payloads
    if (messageProto.payloadtype() == payload::PayloadType::DISCOVERY_REQUEST) {
        const payload::DiscoveryRequest& discoveryRequest =
            messageProto.discoveryrequest();
        payload->deserializeFromProto(discoveryRequest);
    } else if (messageProto.payloadtype() ==
               payload::PayloadType::DISCOVERY_RESPONSE) {
        const payload::DiscoveryResponse& discoveryResponse =
            messageProto.discoveryresponse();
        payload->deserializeFromProto(discoveryResponse);
    } else if (messageProto.payloadtype() ==
               payload::PayloadType::TASK_REQUEST) {
        const payload::TaskRequest& taskRequest = messageProto.taskrequest();
        payload->deserializeFromProto(taskRequest);
    } else if (messageProto.payloadtype() ==
               payload::PayloadType::TASK_RESPONSE) {
        const payload::TaskResponse& taskResponse = messageProto.taskresponse();
        payload->deserializeFromProto(taskResponse);
    } else if (messageProto.payloadtype() ==
               payload::PayloadType::REGISTRATION) {
        const payload::Registration& registration = messageProto.registration();
        payload->deserializeFromProto(registration);
    } else if (messageProto.payloadtype() ==
               payload::PayloadType::REGISTRATION_RESPONSE) {
        const payload::RegistrationResponse&  registrationResponse =
            messageProto.registrationresponse();
        payload->deserializeFromProto(registrationResponse);
    } else if (messageProto.payloadtype() ==
               payload::PayloadType::ACKNOWLEDGEMENT) {
        const payload::Acknowledgement& ack = messageProto.acknowledgement();
        payload->deserializeFromProto(ack);
    } else if (messageProto.payloadtype() ==
               payload::PayloadType::MODEL_STATE_DICT_PARAMS) {
        const payload::ModelStateDictParams& modelParams = messageProto.modelstatedictparams();
        payload->deserializeFromProto(modelParams);
    } else {
        cerr << "Unknown or unsupported payload type for deserialization"
             << endl;
    }
}
