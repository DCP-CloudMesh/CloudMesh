#include "../../include/RequestResponse/task_response.h"

using namespace std;
using namespace nlohmann;

TaskResponse::TaskResponse() : Payload(Type::TASK_RESPONSE) {}
TaskResponse::TaskResponse(const vector<int>& trainingData)
    : Payload(Type::TASK_RESPONSE), trainingData{trainingData} {}

vector<int> TaskResponse::getTrainingData() const { return trainingData; }

string TaskResponse::serialize() const {
    json j;
    j["trainingData"] = trainingData;
    return j.dump();
}

void TaskResponse::deserialize(const string& serializedData) {
    try {
        json j = json::parse(serializedData);
        trainingData = j["trainingData"].get<vector<int>>();
    } catch (json::exception& e) {
        cout << "JSON parsing error: " << e.what() << endl;
    }
}
