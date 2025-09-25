#ifndef SCHEDULE_MANAGER_H
#define SCHEDULE_MANAGER_H

#include <string>
#include <vector>

struct Activity {
    int id;
    std::string title;
    std::string locationName;
    std::string startTime; // 格式：YYYY-MM-DD HH:MM
    std::string description;
};

class ScheduleManager {
private:
    struct Node {
        Activity data;
        Node* left = nullptr;
        Node* right = nullptr;
    };

    Node* root = nullptr;

    static long long toKey(const std::string& timeStr);
    static Node* insert(Node* node, const Activity& activity);
    static void inOrder(Node* node, std::vector<Activity>& out);
    static Node* removeExpired(Node* node, long long key);
    static Node* mergeSubtrees(Node* left, Node* right);
    static void preOrder(Node* node, std::vector<Activity>& out);
    static void destroy(Node* node);

public:
    ~ScheduleManager();

    void addActivity(const Activity& activity);
    std::vector<Activity> generateSchedule() const;
    void clearExpired(const std::string& currentTime);
    std::vector<Activity> backup() const;
    void clear();
    void loadFromActivities(const std::vector<Activity>& activities);
};

#endif // SCHEDULE_MANAGER_H
