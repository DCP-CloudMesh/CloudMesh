#ifndef _TASK_RESPONSE_H
#define _TASK_RESPONSE_H

#include <string>
#include <vector>

#include "../utility.h"
#include "payload.h"

class TaskResponse : public Payload {
    // bytes representing the training data
    std::string modelStateDict;
    bool trainingIsComplete;

  public:
    TaskResponse();
    TaskResponse(const std::string& modelStateDict, bool trainingIsComplete);

    std::string getTrainingData() const;
    void setTrainingData(const std::string& modelStateDict);

    bool getTrainingIsComplete() const;
    void setTrainingIsComplete(bool trainingIsComplete);

    google::protobuf::Message* serializeToProto() const override;
    void deserializeFromProto(
        const google::protobuf::Message& protoMessage) override;
};

#endif
