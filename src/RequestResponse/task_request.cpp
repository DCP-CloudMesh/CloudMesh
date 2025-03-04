#include "../../include/RequestResponse/task_request.h"

using namespace std;

TaskRequest::TaskRequest()
    : Payload(Type::TASK_REQUEST), taskRequestType{NONE} {}

TaskRequest::TaskRequest(const unsigned int numWorkers, const std::string& data,
                         TaskRequestType type)
    : Payload(Type::TASK_REQUEST), numWorkers{numWorkers},
      taskRequestType{type} {
    if (type == GLOB_PATTERN) {
        globPattern = data;
    } else if (type == INDEX_FILENAME) {
        trainingDataIndexFilename = data;
    }
}

void TaskRequest::setLeaderUuid(const string& leaderUuid) {
    this->leaderUuid = leaderUuid;
}

void TaskRequest::setAssignedWorkers(const AddressTable& assignedWorkers) {
    this->assignedWorkers = assignedWorkers;
}

void TaskRequest::setGlobPattern(const std::string& pattern) {
    taskRequestType = GLOB_PATTERN;
    globPattern = pattern;
    trainingDataIndexFilename.clear();
}

void TaskRequest::setTrainingDataIndexFilename(const std::string& filename) {
    taskRequestType = INDEX_FILENAME;
    trainingDataIndexFilename = filename;
    globPattern.clear();
}

void TaskRequest::writeToTrainingDataIndexFile(
    const vector<string>& trainingDataFiles) const {
    fs::path indexFilePath = resolveDataFile(trainingDataIndexFilename);

    ofstream indexFile(indexFilePath, std::ios::out | std::ios::trunc);
    if (!indexFile) {
        // Throw an exception or handle the error appropriately
        cerr << "Failed to open index file: " << indexFilePath << endl;
        throw runtime_error("Could not open training data index file");
    }

    try {
        for (const string& filename : trainingDataFiles) {
            indexFile << filename << endl;

            // Check for write errors after each write
            if (!indexFile) {
                cerr << "Error writing to index file for filename: " << filename
                     << endl;
                throw runtime_error("Write error in training data index file");
            }
        }

        // Explicitly flush to ensure all data is written
        indexFile.flush();
    } catch (const exception& e) {
        cerr << "Exception during index file writing: " << e.what() << endl;
        indexFile.close();
        throw; // Re-throw to allow caller to handle
    }

    indexFile.close();

    // Additional check after closing
    if (indexFile.bad()) {
        cerr << "Possible data corruption during index file writing" << endl;
    }
}

unsigned int TaskRequest::getNumWorkers() const { return numWorkers; }

string TaskRequest::getLeaderUuid() const { return leaderUuid; }

AddressTable TaskRequest::getAssignedWorkers() const { return assignedWorkers; }

std::string TaskRequest::getGlobPattern() const {
    return (taskRequestType == GLOB_PATTERN) ? globPattern : "";
}

std::string TaskRequest::getTrainingDataIndexFilename() const {
    return (taskRequestType == INDEX_FILENAME) ? trainingDataIndexFilename : "";
}

vector<string> TaskRequest::getTrainingDataFiles() const {
    vector<string> trainingDataFiles;

    if (taskRequestType == GLOB_PATTERN) {
        regex pattern = convertToRegexPattern(globPattern);
        trainingDataFiles = getMatchingDataFiles(pattern, DATA_DIR);
    } else if (taskRequestType == INDEX_FILENAME) {
        ifstream indexFile(resolveDataFile(trainingDataIndexFilename));
        if (indexFile.is_open()) {
            string line;
            while (getline(indexFile, line)) {
                trainingDataFiles.push_back(line);
            }
            indexFile.close();
        }
    }

    return trainingDataFiles;
}

google::protobuf::Message* TaskRequest::serializeToProto() const {
    payload::TaskRequest* proto = new payload::TaskRequest();
    proto->set_numworkers(numWorkers);
    proto->set_leaderuuid(leaderUuid);

    if (taskRequestType == GLOB_PATTERN) {
        proto->set_glob_pattern(globPattern);
    } else if (taskRequestType == INDEX_FILENAME) {
        proto->set_training_data_index_filename(trainingDataIndexFilename);
    }

    utility::AddressTable* addressTableProto =
        serializeAddressTable(assignedWorkers);
    proto->set_allocated_assignedworkers(addressTableProto);

    return proto;
}

void TaskRequest::deserializeFromProto(
    const google::protobuf::Message& protoMessage) {

    const auto& proto = dynamic_cast<const payload::TaskRequest&>(protoMessage);

    numWorkers = proto.numworkers();
    leaderUuid = proto.leaderuuid();

    if (proto.has_glob_pattern()) {
        setGlobPattern(proto.glob_pattern());
    } else if (proto.has_training_data_index_filename()) {
        setTrainingDataIndexFilename(proto.training_data_index_filename());
    }

    // Get assigned workers
    assignedWorkers.clear();
    assignedWorkers = deserializeAddressTable(proto.assignedworkers());
}
