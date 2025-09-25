#ifndef SHUTTLE_SERVICE_H
#define SHUTTLE_SERVICE_H

#include "myqueue.h"
#include <string>
#include <unordered_map>
#include <vector>

struct ShuttlePersistentState {
    std::unordered_map<std::string, std::vector<std::string>> queues;  ///< 各站点的候车队列，键为站点名，值为乘客姓名列表
    std::vector<std::string> onboard;                                  ///< 当前在车上的乘客列表
    std::vector<std::string> history;                                  ///< 乘客下车历史记录
};

/**
 * @class ShuttleService
 * @brief 智慧校园班车服务管理类
 * 
 * 提供完整的校园班车服务功能，包括：
 * - 站点管理：添加新的班车站点
 * - 队列管理：乘客排队候车、上车
 * - 乘客管理：跟踪车上乘客、处理下车
 * - 状态查询：获取各站点候车人数、车上乘客等信息
 * - 数据持久化：支持状态的保存和恢复
 * 
 * @note 使用队列(FIFO)方式管理乘客排队，确保公平性
 * @note 支持多站点同时运营
 * @note 线程安全性需要调用方保证
 */
class ShuttleService {
private:
    std::unordered_map<std::string, MyQueue<std::string>> stationQueues;  ///< 各站点的乘客排队队列
    std::vector<std::string> onboardPassengers;                             ///< 当前车上的乘客列表
    std::vector<std::string> dropoffHistory;                                ///< 乘客下车历史记录

public:
    /**
     * @brief 添加新的班车站点
     * @param stationName 站点名称
     * 
     * 为班车系统添加一个新的站点，并为该站点初始化一个空的候车队列。
     * 如果站点已存在，不会重复添加。
     */
    void addStation(const std::string& stationName);
    
    /**
     * @brief 乘客加入指定站点的候车队列
     * @param stationName 站点名称
     * @param passengerName 乘客姓名
     * 
     * 将指定乘客添加到指定站点的候车队列末尾。
     * 如果站点不存在，会自动创建该站点。
     */
    void joinQueue(const std::string& stationName, const std::string& passengerName);
    
    /**
     * @brief 指定站点的排队乘客上车
     * @param stationName 站点名称
     * @return 上车乘客的姓名
     * @throws std::runtime_error 当站点不存在或没有候车乘客时抛出异常
     * 
     * 从指定站点的候车队列前端取出一名乘客，将其添加到车上乘客列表。
     * 遵循先进先出(FIFO)原则，确保排队的公平性。
     */
    std::string leaveQueue(const std::string& stationName);
    
    /**
     * @brief 指定乘客下车
     * @param passengerName 乘客姓名
     * @throws std::runtime_error 当该乘客不在车上时抛出异常
     * 
     * 将指定乘客从车上乘客列表中移除，并添加到下车历史记录中。
     */
    void dropOffPassenger(const std::string& passengerName);
    
    /**
     * @brief 获取指定站点的候车队列长度
     * @param stationName 站点名称
     * @return 该站点的候车人数，如果站点不存在则返回0
     * 
     * 查询指定站点当前有多少乘客在排队候车。
     */
    std::size_t queueSize(const std::string& stationName) const;
    
    /**
     * @brief 获取所有站点及其候车人数的快照
     * @return 包含站点名称和候车人数的配对向量
     * 
     * 返回当前所有站点的状态信息，用于系统监控和统计。
     */
    std::vector<std::pair<std::string, std::size_t>> snapshot() const;
    
    /**
     * @brief 获取当前车上乘客列表
     * @return 车上乘客姓名的向量引用
     * 
     * 返回当前在班车上的所有乘客姓名列表。
     */
    const std::vector<std::string>& onboardList() const;
    
    /**
     * @brief 获取乘客下车历史记录
     * @return 已下车乘客姓名的向量引用
     * 
     * 返回所有已下车乘客的历史记录，按下车时间顺序排列。
     */
    const std::vector<std::string>& dropoffList() const;

    /**
     * @brief 导出当前班车服务状态
     * @return 包含完整状态信息的持久化对象
     * 
     * 将当前班车服务的所有状态信息导出为持久化格式，
     * 包括所有站点队列、车上乘客和下车历史。
     */
    ShuttlePersistentState exportState() const;
    
    /**
     * @brief 从持久化状态加载班车服务数据
     * @param state 包含状态信息的持久化对象
     * 
     * 从持久化状态恢复班车服务的完整状态，
     * 包括重建所有站点队列、车上乘客列表和历史记录。
     * 会清空当前所有数据后重新加载。
     */
    void loadState(const ShuttlePersistentState& state);
};

#endif // SHUTTLE_SERVICE_H
