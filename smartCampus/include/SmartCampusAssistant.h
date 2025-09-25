#ifndef SMART_CAMPUS_ASSISTANT_H
#define SMART_CAMPUS_ASSISTANT_H

#include "LocationManager.h"
#include "NavigationGraph.h"
#include "RankingManager.h"
#include "ScheduleManager.h"
#include "SearchManager.h"
#include "ShuttleService.h"

#include <string>

class SmartCampusAssistant {
private:
    std::string dataDir;
    LocationManager locationManager;
    RankingManager rankingManager;
    SearchManager searchManager;
    ShuttleService shuttleService;
    ScheduleManager scheduleManager;
    NavigationGraph navigationGraph;

    void ensureDataDir() const;
    void loadShuttle();
    void loadSchedule();
    void loadNavigation();
    void saveShuttle() const;
    void saveSchedule() const;
    void saveNavigation() const;

public:
    explicit SmartCampusAssistant(const std::string& dataDirectory);
    ~SmartCampusAssistant();

    void load();
    void save();

    LocationManager& locations();
    RankingManager& ranking();
    SearchManager& search();
    ShuttleService& shuttles();
    ScheduleManager& schedules();
    NavigationGraph& graph();

    const LocationManager& locations() const;
    const RankingManager& ranking() const;
    const SearchManager& search() const;
    const ShuttleService& shuttles() const;
    const ScheduleManager& schedules() const;
    const NavigationGraph& graph() const;
};

#endif // SMART_CAMPUS_ASSISTANT_H
