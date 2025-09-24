#ifndef SMART_CAMPUS_MENU_H
#define SMART_CAMPUS_MENU_H

#include "SmartCampusAssistant.h"

class SmartCampusMenu {
public:
    explicit SmartCampusMenu(SmartCampusAssistant& assistant);

    void run();

private:
    SmartCampusAssistant& assistant;
    LocationManager& locationManager;
    RankingManager& rankingManager;
    SearchManager& searchManager;
    ShuttleService& shuttleService;
    ScheduleManager& scheduleManager;
    NavigationGraph& navigationGraph;

    void showMainMenu() const;
    void handleMainSelection(int choice);

    void showLocationMenu();
    void showShuttleMenu();
    void showScheduleMenu();
    void showNavigationMenu();
};

#endif // SMART_CAMPUS_MENU_H
