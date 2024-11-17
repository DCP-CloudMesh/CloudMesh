#include "../../include/RequestResponse/task_request.h"

using namespace std;
using namespace nlohmann;

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
    ifstream file(trainingFile);
    if (!file.is_open()) {
        cerr << "Error opening file: " << trainingFile << endl;
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
    ofstream file(trainingFile);
    if (!file.is_open()) {
        cerr << "Error opening file: " << trainingFile << endl;
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

string TaskRequest::serialize() const {
    json j;
    j["numWorkers"] = numWorkers;
    j["leaderUuid"] = leaderUuid;
    // Convert unordered_map to JSON object
    for (const auto& entry : assignedWorkers) {
        j["assignedWorkers"][entry.first] = serializeIpAddress(entry.second);
    }
    j["trainingData"] = trainingData;
    j["trainingFile"] = trainingFile;
    return j.dump();
}

void TaskRequest::deserialize(const string& serializedData) {
    try {
        json j = json::parse(serializedData);
        numWorkers = j["numWorkers"].get<unsigned int>();
        leaderUuid = j["leaderUuid"].get<string>();
        trainingData = j["trainingData"].get<vector<int>>();
        trainingFile = j["trainingFile"].get<string>();

        assignedWorkers.clear(); // Clear existing data
        auto workersJson = j["assignedWorkers"];
        for (auto it = workersJson.begin(); it != workersJson.end(); ++it) {
            assignedWorkers[it.key()] = deserializeIpAddress(it.value());
        }
    } catch (json::exception& e) {
        cout << "JSON parsing error: " << e.what() << endl;
    }
}
