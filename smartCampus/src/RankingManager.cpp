#include "RankingManager.h"
#include <iostream>

RankingManager::RankingManager(LocationManager* lm) : locationManager(lm) {}

// 快速排序：按 popularity 从高到低
void RankingManager::quickSort(std::vector<Location>& arr, int left, int right) {
    if (left >= right) return;
    int i = left, j = right;
    Location pivotLoc = arr[left];
    int pivot = pivotLoc.popularity;

    while (i < j) {
        while (i < j && arr[j].popularity <= pivot) --j;
        if (i < j) arr[i++] = arr[j];
        while (i < j && arr[i].popularity >= pivot) ++i;
        if (i < j) arr[j--] = arr[i];
    }
    arr[i] = pivotLoc;
    quickSort(arr, left, i - 1);
    quickSort(arr, i + 1, right);
}

std::vector<Location> RankingManager::rankByPopularity() {
    std::vector<Location> all = locationManager->getAllLocations();
    if (all.empty()) return {};
    quickSort(all, 0, static_cast<int>(all.size()) - 1);
    return all;
}

void RankingManager::printRanking() {
    auto ranking = rankByPopularity();
    std::cout << "===== 热门地点排行榜 =====\n";
    int rank = 1;
    for (const auto& loc : ranking) {
        std::cout << rank++ << ". " << loc.name
            << " (人气: " << loc.popularity << ") - "
            << loc.type << " - " << loc.description << "\n";
    }
}
