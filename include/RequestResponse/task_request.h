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
    // A globPattern for the names of the training
    // data files necessitates the creation of
    // training data index files
    std::string globPattern;
    // Each transported task request should contain an index
    // file that contains the names of the training data files
    std::string trainingDataIndexFilename;

  public:
    // Tagged "union" (to-do: c++17 supports variant class)
    enum TaskRequestType { NONE, GLOB_PATTERN, INDEX_FILENAME } taskRequestType;
    TaskRequest();
    TaskRequest(const unsigned int numWorkers, const std::string& data,
                TaskRequestType type);

    void setLeaderUuid(const std::string& leaderUuid);
    void setAssignedWorkers(const AddressTable& assignedWorkers);
    void setGlobPattern(const std::string& pattern);
    void setTrainingDataIndexFilename(const std::string& filename);

    unsigned int getNumWorkers() const;
    std::string getLeaderUuid() const;
    AddressTable getAssignedWorkers() const;
    std::string getGlobPattern() const;
    std::string getTrainingDataIndexFilename() const;

    google::protobuf::Message* serializeToProto() const override;
    void deserializeFromProto(
        const google::protobuf::Message& protoMessage) override;
};

#endif
