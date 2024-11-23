#include "../../include/RequestResponse/task_request.h"

using namespace std;

TaskRequest::TaskRequest() : Payload(Type::TASK_REQUEST) {}

TaskRequest::TaskRequest(const unsigned int numWorkers,
                         const vector<int>& trainingData)
    : Payload(Type::TASK_REQUEST), numWorkers{numWorkers},
      trainingData{trainingData} {}

TaskRequest::TaskRequest(const unsigned int numWorkers,
                         const std::vector<int>& trainingData,
                         const std::string& trainingFileName)
    : Payload(Type::TASK_REQUEST), numWorkers{numWorkers},
      trainingData{trainingData}, trainingFile{trainingFileName} {

    createTrainingFile();
}

void TaskRequest::setLeaderUuid(const string& leaderUuid) {
    this->leaderUuid = leaderUuid;
}

void TaskRequest::setAssignedWorkers(const AddressTable& assignedWorkers) {
    this->assignedWorkers = assignedWorkers;
}

void TaskRequest::setTrainingData(const vector<int>& trainingData) {
    this->trainingData = trainingData;
}

void TaskRequest::setTrainingFile(const string& trainingFile) {
    this->trainingFile = trainingFile;
}

void TaskRequest::setTrainingDataFromFile() {
    fs::path path = resolveDataFile(trainingFile);
    ifstream file(path.string());
    if (!file.is_open()) {
        cerr << "Error opening file: " << path.string() << endl;
        return;
    }

    trainingData.clear(); // Clear existing data
    string line;
    while (getline(file, line)) {
        trainingData.push_back(stoi(line));
    }
    file.close();
}

void TaskRequest::createTrainingFile() {
    fs::path path = resolveDataFile(trainingFile);
    // create the directory
    fs::create_directories(path.parent_path());
    ofstream file(path.string());
    if (!file.is_open()) {
        cerr << "Error opening file: " << path.string() << endl;
        return;
    }

    for (int i = 0; i < trainingData.size(); i++) {
        file << trainingData[i] << "\n";
    }
    file.close();
}

unsigned int TaskRequest::getNumWorkers() const { return numWorkers; }

vector<int> TaskRequest::getTrainingData() const { return trainingData; }

string TaskRequest::getLeaderUuid() const { return leaderUuid; }

AddressTable TaskRequest::getAssignedWorkers() const { return assignedWorkers; }

string TaskRequest::getTrainingFile() const { return trainingFile; }

google::protobuf::Message* TaskRequest::serializeToProto() const {
    payload::TaskRequest* proto = new payload::TaskRequest();
    proto->set_numworkers(numWorkers);
    proto->set_leaderuuid(leaderUuid);
    proto->set_trainingfile(trainingFile);

    for (const auto& entry : trainingData) {
        proto->add_trainingdata(entry);
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
    trainingFile = proto.trainingfile();

    trainingData.clear();
    for (const auto& entry : proto.trainingdata()) {
        trainingData.push_back(entry);
    }

    // Get assigned workers
    assignedWorkers.clear();
    assignedWorkers = deserializeAddressTable(proto.assignedworkers());
}
