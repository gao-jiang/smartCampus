#ifndef NAVIGATION_GRAPH_H
#define NAVIGATION_GRAPH_H

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class NavigationGraph {
private:
    std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> adjacency;

public:
    void addLocation(const std::string& name);
    void addEdge(const std::string& from, const std::string& to, double distance, bool bidirectional = true);
    bool hasLocation(const std::string& name) const;
    std::vector<std::pair<std::string, double>> neighbors(const std::string& name) const;

    std::vector<std::string> shortestPath(const std::string& start, const std::string& end, double& totalDistance) const;
    std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> exportGraph() const;
    void clear();
};

#endif // NAVIGATION_GRAPH_H
