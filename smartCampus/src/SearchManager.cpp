#include "SearchManager.h"

#include <cctype>

SearchManager::SearchManager(LocationManager* lm) : locationManager(lm) {}

std::string SearchManager::toLower(const std::string& text) {
    std::string res;
    res.reserve(text.size());
    for (unsigned char ch : text) {
        res.push_back(static_cast<char>(std::tolower(ch)));
    }
    return res;
}

std::vector<Location> SearchManager::searchByName(const std::string& keyword) const {
    std::vector<Location> matches;
    if (!locationManager) {
        return matches;
    }
    auto all = locationManager->getAllLocations();
    if (keyword.empty()) {
        return all;
    }
    std::string keyLower = toLower(keyword);
    for (const auto& loc : all) {
        if (toLower(loc.name).find(keyLower) != std::string::npos) {
            matches.push_back(loc);
        }
    }
    return matches;
}
