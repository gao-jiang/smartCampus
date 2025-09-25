#include "SmartCampusMenu.h"

#include <exception>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {
void consumeLine() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            consumeLine();
            return value;
        }
        std::cin.clear();
        consumeLine();
        std::cout << "输入无效，请输入整数。\n";
    }
}

double readDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            consumeLine();
            return value;
        }
        std::cin.clear();
        consumeLine();
        std::cout << "输入无效，请输入数字。\n";
    }
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

std::vector<std::string> collectLocationNames(const LocationManager& manager) {
    std::vector<std::string> names;
    for (const auto& loc : manager.getAllLocations()) {
        names.push_back(loc.name);
    }
    return names;
}

void printOptions(const std::vector<std::string>& options, const std::string& header) {
    if (options.empty()) {
        std::cout << "当前列表为空。\n";
        return;
    }
    if (!header.empty()) {
        std::cout << header << "\n";
    }
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << (i + 1) << ". " << options[i] << "\n";
    }
}

int chooseFromList(const std::vector<std::string>& options, const std::string& prompt) {
    if (options.empty()) {
        return -1;
    }
    while (true) {
        int selection = readInt(prompt);
        if (selection == 0) {
            return -1;
        }
        if (selection >= 1 && static_cast<size_t>(selection) <= options.size()) {
            return selection - 1;
        }
        std::cout << "编号超出范围，请重试。\n";
    }
}
} // namespace

SmartCampusMenu::SmartCampusMenu(SmartCampusAssistant& assistantRef)
    : assistant(assistantRef),
      locationManager(assistant.locations()),
      rankingManager(assistant.ranking()),
      searchManager(assistant.search()),
      shuttleService(assistant.shuttles()),
      scheduleManager(assistant.schedules()),
      navigationGraph(assistant.graph()) {}

void SmartCampusMenu::showMainMenu() const {
    std::cout << "\n===== 智慧校园生活助手 =====\n";
    std::cout << "1. 校园地点管理\n";
    std::cout << "2. 导览车候车服务\n";
    std::cout << "3. 日程与备份管理\n";
    std::cout << "4. 校园导航\n";
    std::cout << "5. 地点搜索\n";
    std::cout << "6. 热门地点排行榜\n";
    std::cout << "7. 退出系统\n";
}

void SmartCampusMenu::handleMainSelection(int choice) {
    switch (choice) {
    case 1:
        showLocationMenu();
        break;
    case 2:
        showShuttleMenu();
        break;
    case 3:
        showScheduleMenu();
        break;
    case 4:
        showNavigationMenu();
        break;
    case 5: {
        std::string keyword = readLine("请输入地点名称关键字：");
        auto results = searchManager.searchByName(keyword);
        if (results.empty()) {
            std::cout << "未找到匹配的地点。\n";
        } else {
            std::cout << "找到 " << results.size() << " 个地点：\n";
            for (const auto& loc : results) {
                std::cout << "ID: " << loc.id << " | " << loc.name << " | 类型: " << loc.type
                          << " | 人气: " << loc.popularity << "\n";
            }
        }
        break;
    }
    case 6: {
        auto ranking = rankingManager.rankByPopularity();
        if (ranking.empty()) {
            std::cout << "暂无地点信息。\n";
        } else {
            std::cout << "===== 热门地点排行榜 =====\n";
            int rank = 1;
            for (const auto& loc : ranking) {
                std::cout << rank++ << ". " << loc.name << " | 人气: " << loc.popularity
                          << " | 类型: " << loc.type << "\n";
            }
        }
        break;
    }
    default:
        std::cout << "无效的选择，请重试。\n";
        break;
    }
}

void SmartCampusMenu::run() {
    while (true) {
        showMainMenu();
        int choice = readInt("请选择操作：");
        if (choice == 7) {
            assistant.save();
            std::cout << "感谢使用，已退出系统。\n";
            break;
        }
        handleMainSelection(choice);
    }
}

void SmartCampusMenu::showLocationMenu() {
    while (true) {
        std::cout << "\n--- 校园地点管理 ---\n";
        std::cout << "1. 查看全部地点\n";
        std::cout << "2. 添加地点\n";
        std::cout << "3. 更新地点\n";
        std::cout << "4. 删除地点\n";
        std::cout << "5. 返回主菜单\n";
        int choice = readInt("请选择操作：");
        if (choice == 1) {
            locationManager.listAll();
        } else if (choice == 2) {
            Location loc{};
            loc.id = readInt("请输入地点ID：");
            loc.name = readLine("请输入地点名称：");
            loc.type = readLine("请输入地点类型：");
            loc.description = readLine("请输入地点描述：");
            loc.popularity = readInt("请输入人气值（整数）：");
            locationManager.addLocation(loc);
            navigationGraph.addLocation(loc.name);
        } else if (choice == 3) {
            int id = readInt("请输入需要更新的ID：");
            Location newInfo{};
            newInfo.id = id;
            newInfo.name = readLine("新名称：");
            newInfo.type = readLine("新类型：");
            newInfo.description = readLine("新描述：");
            newInfo.popularity = readInt("新的人气值：");
            locationManager.updateLocation(id, newInfo);
            if (!newInfo.name.empty()) {
                navigationGraph.addLocation(newInfo.name);
            }
        } else if (choice == 4) {
            int id = readInt("请输入需要删除的ID：");
            locationManager.removeLocation(id);
        } else if (choice == 5) {
            return;
        } else {
            std::cout << "无效的选择，请重试。\n";
        }
    }
}

void SmartCampusMenu::showShuttleMenu() {
    while (true) {
        std::cout << "\n--- 导览车候车服务 ---\n";
        std::cout << "1. 添加站点\n";
        std::cout << "2. 乘客排队\n";
        std::cout << "3. 乘客上车\n";
        std::cout << "4. 乘客下车\n";
        std::cout << "5. 查看导览车状态\n";
        std::cout << "6. 返回主菜单\n";
        int choice = readInt("请选择操作：");
        if (choice == 1) {
            std::string station = readLine("请输入站点名称：");
            shuttleService.addStation(station);
            std::cout << "已创建站点：" << station << "\n";
        } else if (choice == 2) {
            std::string station = readLine("站点名称：");
            std::string passenger = readLine("乘客姓名：");
            shuttleService.joinQueue(station, passenger);
            std::cout << passenger << " 已加入 " << station << " 候车队列。\n";
        } else if (choice == 3) {
            std::string station = readLine("站点名称：");
            try {
                std::string passenger = shuttleService.leaveQueue(station);
                std::cout << passenger << " 已从 " << station << " 队列上车。\n";
            } catch (const std::exception& ex) {
                std::cout << "操作失败：" << ex.what() << "\n";
            }
        } else if (choice == 4) {
            const auto& onboard = shuttleService.onboardList();
            if (onboard.empty()) {
                std::cout << "当前没有在车乘客。\n";
                continue;
            }
            printOptions(onboard, "当前在车乘客：");
            int selected = chooseFromList(onboard, "请输入要下车乘客的编号（0 返回）：");
            if (selected < 0) {
                std::cout << "已取消操作。\n";
                continue;
            }
            try {
                std::string passenger = onboard[static_cast<size_t>(selected)];
                shuttleService.dropOffPassenger(passenger);
                std::cout << passenger << " 已成功下车。\n";
            } catch (const std::exception& ex) {
                std::cout << "操作失败：" << ex.what() << "\n";
            }
        } else if (choice == 5) {
            auto snap = shuttleService.snapshot();
            if (snap.empty()) {
                std::cout << "当前没有任何站点。\n";
            } else {
                std::cout << "--- 候车队列 ---\n";
                for (const auto& entry : snap) {
                    std::cout << entry.first << " 排队人数：" << entry.second << "\n";
                }
            }
            const auto& onboard = shuttleService.onboardList();
            std::cout << "--- 在车乘客 ---\n";
            if (onboard.empty()) {
                std::cout << "无\n";
            } else {
                for (const auto& passenger : onboard) {
                    std::cout << passenger << "\n";
                }
            }
            const auto& history = shuttleService.dropoffList();
            std::cout << "--- 已下车记录 ---\n";
            if (history.empty()) {
                std::cout << "无\n";
            } else {
                for (const auto& passenger : history) {
                    std::cout << passenger << "\n";
                }
            }
        } else if (choice == 6) {
            return;
        } else {
            std::cout << "无效的选择，请重试。\n";
        }
    }
}

void SmartCampusMenu::showScheduleMenu() {
    while (true) {
        std::cout << "\n--- 日程与备份管理 ---\n";
        std::cout << "1. 添加活动\n";
        std::cout << "2. 查看日程表\n";
        std::cout << "3. 清理过期活动\n";
        std::cout << "4. 备份日程\n";
        std::cout << "5. 返回主菜单\n";
        int choice = readInt("请选择操作：");
        if (choice == 1) {
            Activity activity{};
            activity.id = readInt("活动ID：");
            activity.title = readLine("活动名称：");
            activity.locationName = readLine("活动地点：");
            activity.startTime = readLine("开始时间（YYYY-MM-DD HH:MM）：");
            activity.description = readLine("活动描述：");
            try {
                scheduleManager.addActivity(activity);
                std::cout << "活动已添加。\n";
            } catch (const std::exception& ex) {
                std::cout << "添加失败：" << ex.what() << "\n";
            }
        } else if (choice == 2) {
            auto ordered = scheduleManager.generateSchedule();
            if (ordered.empty()) {
                std::cout << "当前没有活动安排。\n";
            } else {
                std::cout << "===== 活动日程 =====\n";
                for (const auto& act : ordered) {
                    std::cout << act.startTime << " | " << act.title << " | " << act.locationName
                              << " | " << act.description << "\n";
                }
            }
        } else if (choice == 3) {
            std::string now = readLine("当前时间（YYYY-MM-DD HH:MM）：");
            try {
                scheduleManager.clearExpired(now);
                std::cout << "已清理过期活动。\n";
            } catch (const std::exception& ex) {
                std::cout << "清理失败：" << ex.what() << "\n";
            }
        } else if (choice == 4) {
            auto backup = scheduleManager.backup();
            if (backup.empty()) {
                std::cout << "当前没有可备份的活动。\n";
            } else {
                std::cout << "备份完成，共 " << backup.size() << " 条活动：\n";
                for (const auto& act : backup) {
                    std::cout << act.startTime << " | " << act.title << " | " << act.locationName << "\n";
                }
            }
        } else if (choice == 5) {
            return;
        } else {
            std::cout << "无效的选择，请重试。\n";
        }
    }
}

void SmartCampusMenu::showNavigationMenu() {
    while (true) {
        std::cout << "\n--- 校园导航 ---\n";
        std::cout << "1. 添加路径\n";
        std::cout << "2. 查询最短路径\n";
        std::cout << "3. 返回主菜单\n";
        int choice = readInt("请选择操作：");
        if (choice == 1) {
            auto names = collectLocationNames(locationManager);
            if (names.size() < 2) {
                std::cout << "地点数量不足，请先在地点管理中添加地点。\n";
                continue;
            }
            printOptions(names, "可选地点：");
            int fromIndex = chooseFromList(names, "请选择起点编号（0 返回）：");
            if (fromIndex < 0) {
                std::cout << "已取消操作。\n";
                continue;
            }
            printOptions(names, "可选地点：");
            int toIndex = chooseFromList(names, "请选择终点编号（0 返回）：");
            if (toIndex < 0) {
                std::cout << "已取消操作。\n";
                continue;
            }
            const std::string& from = names[static_cast<size_t>(fromIndex)];
            const std::string& to = names[static_cast<size_t>(toIndex)];
            double distance = readDouble("路径距离/时间：");
            navigationGraph.addEdge(from, to, distance, true);
            std::cout << "已连接 " << from << " 与 " << to << "，距离/时间：" << distance << "\n";
        } else if (choice == 2) {
            auto names = collectLocationNames(locationManager);
            if (names.size() < 2) {
                std::cout << "地点数量不足，请先添加地点。\n";
                continue;
            }
            printOptions(names, "可选地点如下：");
            int startIndex = chooseFromList(names, "请选择起点编号（0 返回）：");
            if (startIndex < 0) {
                std::cout << "已取消操作。\n";
                continue;
            }
            printOptions(names, "可选地点如下：");
            int endIndex = chooseFromList(names, "请选择终点编号（0 返回）：");
            if (endIndex < 0) {
                std::cout << "已取消操作。\n";
                continue;
            }
            const std::string& start = names[static_cast<size_t>(startIndex)];
            const std::string& end = names[static_cast<size_t>(endIndex)];
            double total = 0.0;
            auto path = navigationGraph.shortestPath(start, end, total);
            if (path.empty()) {
                std::cout << "未找到从 " << start << " 到 " << end << " 的路径。\n";
            } else {
                std::cout << "最优路径（总距离/时间 " << total << "）：";
                for (size_t i = 0; i < path.size(); ++i) {
                    if (i) std::cout << " -> ";
                    std::cout << path[i];
                }
                std::cout << "\n";
            }
        } else if (choice == 3) {
            return;
        } else {
            std::cout << "无效的选择，请重试。\n";
        }
    }
}
