#ifndef _MODEL_STATE_DICT_PARAMS_H
#define _MODEL_STATE_DICT_PARAMS_H

#include <string>
#include <vector>

#include "../utility.h"
#include "payload.h"

class ModelStateDictParams : public Payload {
    // bytes representing the training data
    std::string modelStateDict;
    bool trainingIsComplete;

  public:
    ModelStateDictParams();
    ModelStateDictParams(const std::string& modelStateDict);

    std::string getTrainingData() const;
    void setTrainingData(const std::string& modelStateDict);

    bool getTrainingIsComplete() const;
    void setTrainingIsComplete(bool trainingIsComplete);

    google::protobuf::Message* serializeToProto() const override;
    void deserializeFromProto(
        const google::protobuf::Message& protoMessage) override;
};

#endif
