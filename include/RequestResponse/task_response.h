#ifndef _TASK_RESPONSE_H
#define _TASK_RESPONSE_H

#include <string>
#include <vector>

#include "../utility.h"
#include "payload.h"

class TaskResponse : public Payload {
    std::vector<int> trainingData;

  public:
    TaskResponse();
    TaskResponse(const std::vector<int>& trainingData);

    std::vector<int> getTrainingData() const;
    void setTrainingData(const std::vector<int>& trainingData);

    std::string serialize() const override;
    void deserialize(const std::string& serializedData) override;
};

#endif
