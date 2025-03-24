#include "../../include/RequestResponse/task_response.h"

using namespace std;

TaskResponse::TaskResponse() : Payload(Type::TASK_RESPONSE) {}

TaskResponse::TaskResponse(const string& modelStateDict,
                           bool trainingIsComplete)
    : Payload(Type::TASK_RESPONSE), modelStateDict{modelStateDict},
      trainingIsComplete{trainingIsComplete} {}

string TaskResponse::getTrainingData() const { return modelStateDict; }

void TaskResponse::setTrainingData(const string& modelStateDict) {
    this->modelStateDict = modelStateDict;
}

bool TaskResponse::getTrainingIsComplete() const { return trainingIsComplete; }

void TaskResponse::setTrainingIsComplete(bool trainingIsComplete) {
    this->trainingIsComplete = trainingIsComplete;
}

google::protobuf::Message* TaskResponse::serializeToProto() const {
    payload::TaskResponse* proto = new payload::TaskResponse();
    proto->set_modelstatedict(modelStateDict);
    proto->set_trainingiscomplete(trainingIsComplete);
    return proto;
}

void TaskResponse::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {

    const payload::TaskResponse& proto =
        dynamic_cast<const payload::TaskResponse&>(protoMessage);

    modelStateDict = proto.modelstatedict();
    trainingIsComplete = proto.trainingiscomplete();
}
