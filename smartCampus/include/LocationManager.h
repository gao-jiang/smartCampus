#ifndef LOCATION_MANAGER_H
#define LOCATION_MANAGER_H

#include <string>
#include <vector>

struct Location {
    int id;
    std::string name;
    std::string type;
    std::string description;
    int popularity; // 新增：人气值（访问量/评分等）
};

class LocationManager {
private:
    std::vector<Location> locations;
    std::string filename;

public:
    LocationManager(const std::string& file);

    void loadFromFile();
    void saveToFile();

    void addLocation(const Location& loc);
    void removeLocation(int id);
    void updateLocation(int id, const Location& newInfo);
    Location* findLocation(int id);
    std::vector<Location> getAllLocations() const;
    void listAll();
};

#endif // LOCATION_MANAGER_H
