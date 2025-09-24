#ifndef RANKING_MANAGER_H
#define RANKING_MANAGER_H

#include "LocationManager.h"
#include <vector>

class RankingManager {
private:
    LocationManager* locationManager;
    void quickSort(std::vector<Location>& arr, int left, int right);

public:
    RankingManager(LocationManager* lm);
    std::vector<Location> rankByPopularity();
    void printRanking();
};

#endif // RANKING_MANAGER_H
