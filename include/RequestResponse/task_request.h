#ifndef _TASK_REQUEST_H
#define _TASK_REQUEST_H

#include <fstream>
#include <string>
#include <vector>

#include "../utility.h"
#include "payload.h"

class TaskRequest : public Payload {
    unsigned int numWorkers;
    std::string leaderUuid;
    AddressTable assignedWorkers;
    // A task request can have the training data as an array of integers
    std::vector<int> trainingData;
    // A task request can have a string that specifies the
    // name of the training file on the local filesystem
    std::string trainingFile;
    // Creates a file containing the trainingData as trainingFile
    void createTrainingFile();

  public:
    TaskRequest();
    TaskRequest(const unsigned int numWorkers,
                const std::vector<int>& trainingdata);
    TaskRequest(const unsigned int numWorkers,
                const std::vector<int>& trainingdata,
                const std::string& trainingFile);

    void setLeaderUuid(const std::string& leaderUuid);
    void setAssignedWorkers(const AddressTable& assignedWorkers);
    void setTrainingData(const std::vector<int>& trainingData);
    void setTrainingFile(const std::string& trainingFile);
    void setTrainingDataFromFile();

    unsigned int getNumWorkers() const;
    std::vector<int> getTrainingData() const;
    std::string getLeaderUuid() const;
    AddressTable getAssignedWorkers() const;
    std::string getTrainingFile() const;

    std::string serialize() const override;
    void deserialize(const std::string& serializedData) override;
};

#endif
