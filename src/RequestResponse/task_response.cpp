#include "../../include/RequestResponse/task_response.h"

using namespace std;

TaskResponse::TaskResponse() : Payload(Type::TASK_RESPONSE) {}
TaskResponse::TaskResponse(const vector<int>& trainingData)
    : Payload(Type::TASK_RESPONSE), trainingData{trainingData} {}

vector<int> TaskResponse::getTrainingData() const { return trainingData; }

void TaskResponse::setTrainingData(const vector<int>& trainingData) {
    this->trainingData = trainingData;
}

google::protobuf::Message* TaskResponse::serializeToProto() const {
    payload::TaskResponse* proto = new payload::TaskResponse();

    for (const auto& value : trainingData) {
        proto->add_trainingdata(value);
    }

    return proto;
}

void TaskResponse::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {

    const payload::TaskResponse& proto =
        dynamic_cast<const payload::TaskResponse&>(protoMessage);

    trainingData.clear();
    for (const auto& value : proto.trainingdata()) {
        trainingData.push_back(value);
    }
}
