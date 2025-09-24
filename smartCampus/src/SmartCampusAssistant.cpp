#include "SmartCampusAssistant.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace {
constexpr const char* kShuttleFile = "shuttle.txt";
constexpr const char* kScheduleFile = "schedule.txt";
constexpr const char* kNavigationFile = "navigation.txt";
constexpr const char* kLocationsFile = "locations.txt";

std::string stripBom(std::string text) {
    static const std::string bom = "\xEF\xBB\xBF";
    if (text.rfind(bom, 0) == 0) {
        text.erase(0, bom.size());
    }
    return text;
}

void trimTrailingCarriageReturn(std::string& line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
}
} // namespace

SmartCampusAssistant::SmartCampusAssistant(const std::string& dataDirectory)
    : dataDir(dataDirectory),
      locationManager((std::filesystem::path(dataDirectory) / kLocationsFile).string()),
      rankingManager(&locationManager),
      searchManager(&locationManager) {
    ensureDataDir();
    load();
}

SmartCampusAssistant::~SmartCampusAssistant() {
    try {
        save();
    } catch (const std::exception& ex) {
        std::cerr << "保存数据时出现异常：" << ex.what() << "\n";
    }
}

void SmartCampusAssistant::ensureDataDir() const {
    try {
        std::filesystem::path path(dataDir);
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
    } catch (const std::exception& ex) {
        std::cerr << "无法确保数据目录存在：" << ex.what() << "\n";
    }
}

void SmartCampusAssistant::load() {
    locationManager.loadFromFile();
    loadShuttle();
    loadSchedule();
    loadNavigation();
}

void SmartCampusAssistant::save() {
    ensureDataDir();
    locationManager.saveToFile();
    saveShuttle();
    saveSchedule();
    saveNavigation();
}

LocationManager& SmartCampusAssistant::locations() {
    return locationManager;
}

RankingManager& SmartCampusAssistant::ranking() {
    return rankingManager;
}

SearchManager& SmartCampusAssistant::search() {
    return searchManager;
}

ShuttleService& SmartCampusAssistant::shuttles() {
    return shuttleService;
}

ScheduleManager& SmartCampusAssistant::schedules() {
    return scheduleManager;
}

NavigationGraph& SmartCampusAssistant::graph() {
    return navigationGraph;
}

const LocationManager& SmartCampusAssistant::locations() const {
    return locationManager;
}

const RankingManager& SmartCampusAssistant::ranking() const {
    return rankingManager;
}

const SearchManager& SmartCampusAssistant::search() const {
    return searchManager;
}

const ShuttleService& SmartCampusAssistant::shuttles() const {
    return shuttleService;
}

const ScheduleManager& SmartCampusAssistant::schedules() const {
    return scheduleManager;
}

const NavigationGraph& SmartCampusAssistant::graph() const {
    return navigationGraph;
}

void SmartCampusAssistant::loadShuttle() {
    std::filesystem::path file = std::filesystem::path(dataDir) / kShuttleFile;
    std::ifstream fin(file);
    if (!fin.is_open()) {
        shuttleService.loadState(ShuttlePersistentState{});
        return;
    }

    ShuttlePersistentState state;
    std::string line;
    auto parsePassengers = [](const std::string& passengersStr) {
        std::vector<std::string> passengers;
        std::stringstream ss(passengersStr);
        std::string token;
        while (std::getline(ss, token, ',')) {
            if (!token.empty()) {
                passengers.push_back(token);
            }
        }
        return passengers;
    };

    while (std::getline(fin, line)) {
        trimTrailingCarriageReturn(line);
        if (line.empty()) {
            continue;
        }
        line = stripBom(line);
        bool hasTag = line.size() > 2 && line[1] == '|' && (line[0] == 'Q' || line[0] == 'O' || line[0] == 'D');
        if (!hasTag) {
            auto pos = line.find('|');
            if (pos == std::string::npos) {
                continue;
            }
            std::string station = stripBom(line.substr(0, pos));
            std::string passengersStr = line.substr(pos + 1);
            state.queues.emplace(std::move(station), parsePassengers(passengersStr));
            continue;
        }

        char tag = line[0];
        std::string rest = line.substr(2);
        if (tag == 'Q') {
            auto pos = rest.find('|');
            if (pos == std::string::npos) {
                continue;
            }
            std::string station = rest.substr(0, pos);
            std::string passengersStr = rest.substr(pos + 1);
            state.queues[stripBom(station)] = parsePassengers(passengersStr);
        } else if (tag == 'O') {
            state.onboard = parsePassengers(rest);
        } else if (tag == 'D') {
            state.history = parsePassengers(rest);
        }
    }

    shuttleService.loadState(state);
}

void SmartCampusAssistant::saveShuttle() const {
    std::filesystem::path file = std::filesystem::path(dataDir) / kShuttleFile;
    std::ofstream fout(file, std::ios::trunc);
    if (!fout.is_open()) {
        std::cerr << "错误：无法写入导览车数据文件：" << file.string() << "\n";
        return;
    }

    ShuttlePersistentState state = shuttleService.exportState();
    for (const auto& kv : state.queues) {
        fout << "Q|" << kv.first << "|";
        for (size_t i = 0; i < kv.second.size(); ++i) {
            if (i) fout << ',';
            fout << kv.second[i];
        }
        fout << "\n";
    }
    fout << "O|";
    for (size_t i = 0; i < state.onboard.size(); ++i) {
        if (i) fout << ',';
        fout << state.onboard[i];
    }
    fout << "\n";

    fout << "D|";
    for (size_t i = 0; i < state.history.size(); ++i) {
        if (i) fout << ',';
        fout << state.history[i];
    }
    fout << "\n";
}

void SmartCampusAssistant::loadSchedule() {
    std::filesystem::path file = std::filesystem::path(dataDir) / kScheduleFile;
    std::ifstream fin(file);
    if (!fin.is_open()) {
        scheduleManager.clear();
        return;
    }

    std::vector<Activity> activities;
    std::string line;
    while (std::getline(fin, line)) {
        trimTrailingCarriageReturn(line);
        if (line.empty()) {
            continue;
        }
        line = stripBom(line);
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> parts;
        while (std::getline(ss, token, '|')) {
            parts.push_back(token);
        }
        if (parts.size() < 5) {
            continue;
        }
        try {
            Activity act{};
            act.id = std::stoi(parts[0]);
            act.title = parts[1];
            act.locationName = parts[2];
            act.startTime = parts[3];
            act.description = parts[4];
            activities.push_back(std::move(act));
        } catch (...) {
            continue;
        }
    }

    scheduleManager.loadFromActivities(activities);
}

void SmartCampusAssistant::saveSchedule() const {
    std::filesystem::path file = std::filesystem::path(dataDir) / kScheduleFile;
    std::ofstream fout(file, std::ios::trunc);
    if (!fout.is_open()) {
        std::cerr << "错误：无法写入日程数据文件：" << file.string() << "\n";
        return;
    }

    auto ordered = scheduleManager.generateSchedule();
    for (const auto& act : ordered) {
        fout << act.id << "|" << act.title << "|" << act.locationName
             << "|" << act.startTime << "|" << act.description << "\n";
    }
}

void SmartCampusAssistant::loadNavigation() {
    navigationGraph.clear();
    for (const auto& loc : locationManager.getAllLocations()) {
        navigationGraph.addLocation(loc.name);
    }

    std::filesystem::path file = std::filesystem::path(dataDir) / kNavigationFile;
    std::ifstream fin(file);
    if (!fin.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(fin, line)) {
        trimTrailingCarriageReturn(line);
        if (line.empty()) {
            continue;
        }
        line = stripBom(line);
        std::stringstream ss(line);
        std::string recordType;
        if (!std::getline(ss, recordType, '|')) {
            continue;
        }
        if (recordType == "N") {
            std::string nodeName;
            if (std::getline(ss, nodeName)) {
                navigationGraph.addLocation(nodeName);
            }
        } else if (recordType == "E") {
            std::string from;
            std::string to;
            std::string distanceStr;
            if (!std::getline(ss, from, '|')) continue;
            if (!std::getline(ss, to, '|')) continue;
            if (!std::getline(ss, distanceStr)) continue;
            try {
                double distance = std::stod(distanceStr);
                navigationGraph.addEdge(from, to, distance, true);
            } catch (...) {
                continue;
            }
        }
    }
}

void SmartCampusAssistant::saveNavigation() const {
    std::filesystem::path file = std::filesystem::path(dataDir) / kNavigationFile;
    std::ofstream fout(file, std::ios::trunc);
    if (!fout.is_open()) {
        std::cerr << "错误：无法写入导航数据文件：" << file.string() << "\n";
        return;
    }

    auto adjacency = navigationGraph.exportGraph();
    std::set<std::string> nodes;
    for (const auto& kv : adjacency) {
        nodes.insert(kv.first);
        for (const auto& neighbor : kv.second) {
            nodes.insert(neighbor.first);
        }
    }
    for (const auto& node : nodes) {
        fout << "N|" << node << "\n";
    }

    std::map<std::pair<std::string, std::string>, double> edges;
    for (const auto& kv : adjacency) {
        const auto& from = kv.first;
        for (const auto& [to, distance] : kv.second) {
            if (from == to) {
                edges[{from, to}] = distance;
                continue;
            }
            std::pair<std::string, std::string> key;
            if (from < to) {
                key = { from, to };
            } else {
                key = { to, from };
            }
            auto it = edges.find(key);
            if (it == edges.end()) {
                edges.emplace(key, distance);
            }
        }
    }

    for (const auto& entry : edges) {
        fout << "E|" << entry.first.first << "|" << entry.first.second << "|" << entry.second << "\n";
    }
}
