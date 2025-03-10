#include "../../include/RequestResponse/model_state_dict_params.h"

using namespace std;

ModelStateDictParams::ModelStateDictParams() : Payload(Type::MODEL_STATE_DICT_PARAMS) {}

ModelStateDictParams::ModelStateDictParams(const string& modelStateDict)
    : Payload(Type::MODEL_STATE_DICT_PARAMS), modelStateDict{modelStateDict} {}

string ModelStateDictParams::getTrainingData() const { return modelStateDict; }

void ModelStateDictParams::setTrainingData(const string& modelStateDict) {
    this->modelStateDict = modelStateDict;
}

google::protobuf::Message* ModelStateDictParams::serializeToProto() const {
    payload::ModelStateDictParams* proto = new payload::ModelStateDictParams();
    proto->set_modelstatedict(modelStateDict);
    return proto;
}

void ModelStateDictParams::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {

    const payload::ModelStateDictParams& proto =
        dynamic_cast<const payload::ModelStateDictParams&>(protoMessage);

    modelStateDict = proto.modelstatedict();
}
