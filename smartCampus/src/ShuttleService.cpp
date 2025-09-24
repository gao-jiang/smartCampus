#include "ShuttleService.h"

#include <algorithm>
#include <stdexcept>

void ShuttleService::addStation(const std::string& stationName) {
    stationQueues.emplace(stationName, std::queue<std::string>());
}

void ShuttleService::joinQueue(const std::string& stationName, const std::string& passengerName) {
    if (!stationQueues.count(stationName)) {
        addStation(stationName);
    }
    stationQueues[stationName].push(passengerName);
}

std::string ShuttleService::leaveQueue(const std::string& stationName) {
    auto it = stationQueues.find(stationName);
    if (it == stationQueues.end() || it->second.empty()) {
        throw std::runtime_error("当前站点没有候车乘客");
    }
    std::string passenger = it->second.front();
    it->second.pop();
    onboardPassengers.push_back(passenger);
    return passenger;
}

std::size_t ShuttleService::queueSize(const std::string& stationName) const {
    auto it = stationQueues.find(stationName);
    if (it == stationQueues.end()) {
        return 0;
    }
    return it->second.size();
}

std::vector<std::pair<std::string, std::size_t>> ShuttleService::snapshot() const {
    std::vector<std::pair<std::string, std::size_t>> res;
    res.reserve(stationQueues.size());
    for (const auto& kv : stationQueues) {
        res.emplace_back(kv.first, kv.second.size());
    }
    return res;
}

void ShuttleService::dropOffPassenger(const std::string& passengerName) {
    auto it = std::find(onboardPassengers.begin(), onboardPassengers.end(), passengerName);
    if (it == onboardPassengers.end()) {
        throw std::runtime_error("该乘客不在车上");
    }
    dropoffHistory.push_back(passengerName);
    onboardPassengers.erase(it);
}

const std::vector<std::string>& ShuttleService::onboardList() const {
    return onboardPassengers;
}

const std::vector<std::string>& ShuttleService::dropoffList() const {
    return dropoffHistory;
}

ShuttlePersistentState ShuttleService::exportState() const {
    ShuttlePersistentState state;
    state.queues.reserve(stationQueues.size());
    for (const auto& kv : stationQueues) {
        std::queue<std::string> copy = kv.second;
        std::vector<std::string> passengers;
        passengers.reserve(copy.size());
        while (!copy.empty()) {
            passengers.push_back(copy.front());
            copy.pop();
        }
        state.queues.emplace(kv.first, std::move(passengers));
    }
    state.onboard = onboardPassengers;
    state.history = dropoffHistory;
    return state;
}

void ShuttleService::loadState(const ShuttlePersistentState& state) {
    stationQueues.clear();
    onboardPassengers = state.onboard;
    dropoffHistory = state.history;
    for (const auto& kv : state.queues) {
        std::queue<std::string> q;
        for (const auto& passenger : kv.second) {
            q.push(passenger);
        }
        stationQueues.emplace(kv.first, std::move(q));
    }
}
