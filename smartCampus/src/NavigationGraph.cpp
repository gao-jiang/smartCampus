#include "NavigationGraph.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>

void NavigationGraph::addLocation(const std::string& name) {
    adjacency.emplace(name, std::vector<std::pair<std::string, double>>());
}

void NavigationGraph::addEdge(const std::string& from, const std::string& to, double distance, bool bidirectional) {
    if (!hasLocation(from)) {
        addLocation(from);
    }
    if (!hasLocation(to)) {
        addLocation(to);
    }
    adjacency[from].emplace_back(to, distance);
    if (bidirectional) {
        adjacency[to].emplace_back(from, distance);
    }
}

bool NavigationGraph::hasLocation(const std::string& name) const {
    return adjacency.count(name) > 0;
}

std::vector<std::pair<std::string, double>> NavigationGraph::neighbors(const std::string& name) const {
    auto it = adjacency.find(name);
    if (it == adjacency.end()) {
        return {};
    }
    return it->second;
}

std::vector<std::string> NavigationGraph::shortestPath(const std::string& start, const std::string& end, double& totalDistance) const {
    totalDistance = 0.0;
    if (!hasLocation(start) || !hasLocation(end)) {
        return {};
    }
    if (start == end) {
        return { start };
    }

    using Entry = std::pair<double, std::string>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> pq;
    std::unordered_map<std::string, double> dist;
    std::unordered_map<std::string, std::string> prev;

    const double inf = std::numeric_limits<double>::infinity();
    for (const auto& kv : adjacency) {
        dist[kv.first] = inf;
    }
    dist[start] = 0.0;
    pq.emplace(0.0, start);

    while (!pq.empty()) {
        auto [currentDist, node] = pq.top();
        pq.pop();
        if (currentDist > dist[node]) {
            continue;
        }
        if (node == end) {
            break;
        }
        auto it = adjacency.find(node);
        if (it == adjacency.end()) {
            continue;
        }
        for (const auto& [neighbor, weight] : it->second) {
            double candidate = currentDist + weight;
            if (candidate < dist[neighbor]) {
                dist[neighbor] = candidate;
                prev[neighbor] = node;
                pq.emplace(candidate, neighbor);
            }
        }
    }

    if (dist[end] == inf) {
        return {};
    }
    totalDistance = dist[end];

    std::vector<std::string> path;
    for (std::string at = end; !at.empty();) {
        path.push_back(at);
        auto it = prev.find(at);
        if (it == prev.end()) {
            break;
        }
        at = it->second;
    }
    if (path.back() != start) {
        return {};
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> NavigationGraph::exportGraph() const {
    return adjacency;
}

void NavigationGraph::clear() {
    adjacency.clear();
}
