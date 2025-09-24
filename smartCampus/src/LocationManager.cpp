#include "LocationManager.h"
#include <iostream>
#include <fstream>
#include <sstream>

static std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> res;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        res.push_back(item);
    }
    return res;
}

LocationManager::LocationManager(const std::string& file) : filename(file) {
    loadFromFile();
}

void LocationManager::loadFromFile() {
    locations.clear();
    std::ifstream fin(filename, std::ios::binary);
    if (!fin.is_open()) {
        std::cout << "提示：未找到数据文件 '" << filename << "'，将创建新文件（首次保存时）。\n";
        return;
    }

    std::string line;
    while (std::getline(fin, line)) {
        // 去掉 Windows 回车
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        // 分割
        auto parts = split(line, '|');
        if (parts.size() < 4) continue; // 格式不对，跳过

        // 处理 BOM（只有可能在第一个字段）
        const std::string bom = "\xEF\xBB\xBF";
        if (parts[0].rfind(bom, 0) == 0) {
            parts[0] = parts[0].substr(bom.size());
        }

        try {
            int id = std::stoi(parts[0]);
            std::string name = parts[1];
            std::string type = parts[2];
            std::string desc = parts[3];
            int popularity = 0;
            if (parts.size() >= 5 && !parts[4].empty()) {
                try { popularity = std::stoi(parts[4]); }
                catch (...) { popularity = 0; }
            }
            Location loc{ id, name, type, desc, popularity };
            locations.push_back(loc);
        }
        catch (...) {
            std::cerr << "警告：解析行失败，跳过 -> " << line << "\n";
            continue;
        }
    }

    fin.close();
}

void LocationManager::saveToFile() {
    std::ofstream fout(filename, std::ios::trunc);
    if (!fout.is_open()) {
        std::cerr << "错误：无法写入文件 '" << filename << "'\n";
        return;
    }
    for (const auto& l : locations) {
        fout << l.id << "|" << l.name << "|" << l.type << "|" << l.description << "|" << l.popularity << "\n";
    }
    fout.close();
}

void LocationManager::addLocation(const Location& loc) {
    for (const auto& l : locations) {
        if (l.id == loc.id) {
            std::cout << "添加失败：ID " << loc.id << " 已存在！\n";
            return;
        }
    }
    locations.push_back(loc);
    saveToFile();
    std::cout << "成功添加地点：" << loc.name << "\n";
}

void LocationManager::removeLocation(int id) {
    for (auto it = locations.begin(); it != locations.end(); ++it) {
        if (it->id == id) {
            std::cout << "已删除地点：" << it->name << "\n";
            locations.erase(it);
            saveToFile();
            return;
        }
    }
    std::cout << "删除失败：未找到ID " << id << "\n";
}

void LocationManager::updateLocation(int id, const Location& newInfo) {
    for (auto& l : locations) {
        if (l.id == id) {
            l.name = newInfo.name;
            l.type = newInfo.type;
            l.description = newInfo.description;
            l.popularity = newInfo.popularity;
            saveToFile();
            std::cout << "已更新地点：" << l.name << "\n";
            return;
        }
    }
    std::cout << "更新失败：未找到ID " << id << "\n";
}

Location* LocationManager::findLocation(int id) {
    for (auto& l : locations) {
        if (l.id == id) return &l;
    }
    return nullptr;
}

std::vector<Location> LocationManager::getAllLocations() const {
    return locations;
}

void LocationManager::listAll() {
    if (locations.empty()) {
        std::cout << "当前没有任何地点信息。\n";
        return;
    }
    std::cout << "===== 校园地点数据库 =====\n";
    for (const auto& l : locations) {
        std::cout << "ID: " << l.id
            << " | 名称: " << l.name
            << " | 类型: " << l.type
            << " | 描述: " << l.description
            << " | 人气: " << l.popularity
            << "\n";
    }
}
