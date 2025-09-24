#include "ScheduleManager.h"

#include <cctype>
#include <stdexcept>

long long ScheduleManager::toKey(const std::string& timeStr) {
    std::string digits;
    digits.reserve(timeStr.size());
    for (char ch : timeStr) {
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            digits.push_back(ch);
        }
    }
    if (digits.size() < 12) {
        throw std::invalid_argument("时间格式应为 YYYY-MM-DD HH:MM");
    }
    int year = std::stoi(digits.substr(0, 4));
    int month = std::stoi(digits.substr(4, 2));
    int day = std::stoi(digits.substr(6, 2));
    int hour = std::stoi(digits.substr(8, 2));
    int minute = std::stoi(digits.substr(10, 2));
    if (month < 1 || month > 12 || day < 1 || day > 31 || hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        throw std::invalid_argument("时间字段取值非法");
    }
    return static_cast<long long>(year) * 100000000LL +
           static_cast<long long>(month) * 1000000LL +
           static_cast<long long>(day) * 10000LL +
           static_cast<long long>(hour) * 100LL +
           static_cast<long long>(minute);
}

ScheduleManager::Node* ScheduleManager::insert(Node* node, const Activity& activity) {
    if (!node) {
        Node* newNode = new Node();
        newNode->data = activity;
        return newNode;
    }
    long long key = toKey(activity.startTime);
    long long currentKey = toKey(node->data.startTime);
    if (key < currentKey || (key == currentKey && activity.id < node->data.id)) {
        node->left = insert(node->left, activity);
    } else {
        node->right = insert(node->right, activity);
    }
    return node;
}

void ScheduleManager::inOrder(Node* node, std::vector<Activity>& out) {
    if (!node) return;
    inOrder(node->left, out);
    out.push_back(node->data);
    inOrder(node->right, out);
}

ScheduleManager::Node* ScheduleManager::mergeSubtrees(Node* left, Node* right) {
    if (!left) return right;
    if (!right) return left;
    Node* cursor = left;
    while (cursor->right) {
        cursor = cursor->right;
    }
    cursor->right = right;
    return left;
}

ScheduleManager::Node* ScheduleManager::removeExpired(Node* node, long long key) {
    if (!node) return nullptr;
    node->left = removeExpired(node->left, key);
    node->right = removeExpired(node->right, key);
    long long nodeKey = toKey(node->data.startTime);
    if (nodeKey < key) {
        Node* merged = mergeSubtrees(node->left, node->right);
        delete node;
        return merged;
    }
    return node;
}

void ScheduleManager::preOrder(Node* node, std::vector<Activity>& out) {
    if (!node) return;
    out.push_back(node->data);
    preOrder(node->left, out);
    preOrder(node->right, out);
}

void ScheduleManager::destroy(Node* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

ScheduleManager::~ScheduleManager() {
    clear();
}

void ScheduleManager::addActivity(const Activity& activity) {
    root = insert(root, activity);
}

std::vector<Activity> ScheduleManager::generateSchedule() const {
    std::vector<Activity> ordered;
    ordered.reserve(16);
    inOrder(root, ordered);
    return ordered;
}

void ScheduleManager::clearExpired(const std::string& currentTime) {
    long long key = toKey(currentTime);
    root = removeExpired(root, key);
}

std::vector<Activity> ScheduleManager::backup() const {
    std::vector<Activity> copy;
    copy.reserve(16);
    preOrder(root, copy);
    return copy;
}

void ScheduleManager::clear() {
    destroy(root);
    root = nullptr;
}

void ScheduleManager::loadFromActivities(const std::vector<Activity>& activities) {
    clear();
    for (const auto& activity : activities) {
        addActivity(activity);
    }
}
