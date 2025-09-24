#ifndef SEARCH_MANAGER_H
#define SEARCH_MANAGER_H

#include "LocationManager.h"
#include <string>
#include <vector>

class SearchManager {
private:
    LocationManager* locationManager;

    static std::string toLower(const std::string& text);

public:
    explicit SearchManager(LocationManager* lm);

    std::vector<Location> searchByName(const std::string& keyword) const;
};

#endif // SEARCH_MANAGER_H
