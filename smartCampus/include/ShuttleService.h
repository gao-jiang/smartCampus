#ifndef SHUTTLE_SERVICE_H
#define SHUTTLE_SERVICE_H

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

struct ShuttlePersistentState {
    std::unordered_map<std::string, std::vector<std::string>> queues;
    std::vector<std::string> onboard;
    std::vector<std::string> history;
};

class ShuttleService {
private:
    std::unordered_map<std::string, std::queue<std::string>> stationQueues;
    std::vector<std::string> onboardPassengers;
    std::vector<std::string> dropoffHistory;

public:
    void addStation(const std::string& stationName);
    void joinQueue(const std::string& stationName, const std::string& passengerName);
    std::string leaveQueue(const std::string& stationName);
    void dropOffPassenger(const std::string& passengerName);
    std::size_t queueSize(const std::string& stationName) const;
    std::vector<std::pair<std::string, std::size_t>> snapshot() const;
    const std::vector<std::string>& onboardList() const;
    const std::vector<std::string>& dropoffList() const;

    ShuttlePersistentState exportState() const;
    void loadState(const ShuttlePersistentState& state);
};

#endif // SHUTTLE_SERVICE_H
