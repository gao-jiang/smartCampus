// 智慧校园班车服务实现文件
// 提供校园班车站点管理、乘客排队、上下车等功能

#include "ShuttleService.h"

#include <algorithm>  // 算法库，用于查找等操作
#include <stdexcept>  // 异常处理库

// 添加新的班车站点
// 参数：stationName - 站点名称
void ShuttleService::addStation(const std::string& stationName) {
    // 为新站点创建一个空的乘客队列
    stationQueues.emplace(stationName, MyQueue<std::string>());
}

// 乘客加入指定站点的候车队列
// 参数：stationName - 站点名称
//       passengerName - 乘客姓名
void ShuttleService::joinQueue(const std::string& stationName, const std::string& passengerName) {
    // 如果站点不存在，先创建该站点
    if (!stationQueues.count(stationName)) {
        addStation(stationName);
    }
    // 将乘客加入对应站点的队列末尾
    stationQueues[stationName].push(passengerName);
}

// 指定站点的排队乘客上车（从队列前端取出一名乘客）
// 参数：stationName - 站点名称
// 返回值：上车乘客的姓名
// 异常：如果站点不存在或没有候车乘客，抛出runtime_error异常
std::string ShuttleService::leaveQueue(const std::string& stationName) {
    // 查找指定站点
    auto it = stationQueues.find(stationName);
    // 检查站点是否存在且有候车乘客
    if (it == stationQueues.end() || it->second.empty()) {
        throw std::runtime_error("当前站点没有候车乘客");
    }
    // 获取队列前端的乘客
    std::string passenger = it->second.front();
    // 从队列中移除该乘客
    it->second.pop();
    // 将乘客加入车上乘客列表
    onboardPassengers.push_back(passenger);
    return passenger;
}

// 获取指定站点的候车队列长度
// 参数：stationName - 站点名称
// 返回值：该站点的候车人数，如果站点不存在则返回0
std::size_t ShuttleService::queueSize(const std::string& stationName) const {
    // 查找指定站点
    auto it = stationQueues.find(stationName);
    // 如果站点不存在，返回0
    if (it == stationQueues.end()) {
        return 0;
    }
    // 返回队列大小
    return it->second.size();
}

// 获取所有站点及其候车人数的快照
// 返回值：包含站点名称和候车人数的配对向量
std::vector<std::pair<std::string, std::size_t>> ShuttleService::snapshot() const {
    std::vector<std::pair<std::string, std::size_t>> res;
    // 预留空间以提高效率
    res.reserve(stationQueues.size());
    // 遍历所有站点，收集站点名称和队列长度
    for (const auto& kv : stationQueues) {
        res.emplace_back(kv.first, kv.second.size());
    }
    return res;
}

// 乘客下车
// 参数：passengerName - 乘客姓名
// 异常：如果该乘客不在车上，抛出runtime_error异常
void ShuttleService::dropOffPassenger(const std::string& passengerName) {
    // 在车上乘客列表中查找指定乘客
    auto it = std::find(onboardPassengers.begin(), onboardPassengers.end(), passengerName);
    // 如果乘客不在车上，抛出异常
    if (it == onboardPassengers.end()) {
        throw std::runtime_error("该乘客不在车上");
    }
    // 将乘客加入下车历史记录
    dropoffHistory.push_back(passengerName);
    // 从车上乘客列表中移除该乘客
    onboardPassengers.erase(it);
}

// 获取当前车上乘客列表
// 返回值：车上乘客姓名的向量引用
const std::vector<std::string>& ShuttleService::onboardList() const {
    return onboardPassengers;
}

// 获取乘客下车历史记录
// 返回值：已下车乘客姓名的向量引用
const std::vector<std::string>& ShuttleService::dropoffList() const {
    return dropoffHistory;
}

// 导出班车服务的当前状态，用于数据持久化
// 返回值：包含所有站点队列、车上乘客和历史记录的状态对象
ShuttlePersistentState ShuttleService::exportState() const {
    ShuttlePersistentState state;
    // 预留空间以提高效率
    state.queues.reserve(stationQueues.size());
    
    // 遍历所有站点队列，将队列转换为向量格式
    for (const auto& kv : stationQueues) {
        // 复制队列以避免修改原队列
        MyQueue<std::string> copy = kv.second;
        std::vector<std::string> passengers;
        passengers.reserve(copy.size());
        
        // 将队列中的所有乘客转移到向量中
        while (!copy.empty()) {
            passengers.push_back(copy.front());
            copy.pop();
        }
        // 将站点和乘客列表加入状态对象
        state.queues.emplace(kv.first, std::move(passengers));
    }
    
    // 保存车上乘客和下车历史
    state.onboard = onboardPassengers;
    state.history = dropoffHistory;
    return state;
}

// 从持久化状态加载班车服务数据
// 参数：state - 包含所有状态信息的持久化对象
void ShuttleService::loadState(const ShuttlePersistentState& state) {
    // 清空当前所有站点队列
    stationQueues.clear();
    
    // 恢复车上乘客和历史记录
    onboardPassengers = state.onboard;
    dropoffHistory = state.history;
    
    // 恢复所有站点的候车队列
    for (const auto& kv : state.queues) {
        MyQueue<std::string> q;
        // 将向量中的乘客依次加入队列
        for (const auto& passenger : kv.second) {
            q.push(passenger);
        }
        // 将重建的队列加入站点管理器
        stationQueues.emplace(kv.first, std::move(q));
    }
}
