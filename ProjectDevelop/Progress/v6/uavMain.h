/**
* @file uavMain.h
* @author Makaka
* @date 2023-12-05
* @brief 定义结构体与头文件
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue> 

struct DataPackHeader
{
    uint16_t packetType;   // 包类型
    uint16_t packetLength; // 包长度
    uint8_t chanelVersion; // 信道/版本
    uint8_t packetPriority; // 优先级
    uint16_t packetSerialNum; // 包序列号
    uint16_t packetIdentificationNum; // 包标识号
    uint16_t packetOffset; // 包偏移量
    uint32_t validTime; // 有效时间
    uint16_t sourceId; // 源ID
    uint16_t targetId; // 目标ID
    uint16_t singleHopSourceId; // 单跳源ID
    uint16_t nextHopId; // 下一跳ID
    uint8_t clusterId; // 集群ID
    uint8_t moduleId; // 模块ID
    uint16_t checkSum; // 校验和
    uint32_t alternateFields; // 备用字段
};

struct DataPack {
    DataPackHeader header; // 包头
    uint8_t* payload; // 数据

    // 重载 < 运算符(与std::less对应)
    bool operator<(const DataPack& other) const {
        return header.packetPriority > other.header.packetPriority;
    }
};

/**
* @brief 模板函数重写push与pop函数，用于多线程保护
* @param None
* @return None
*/
template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }
    // 向队列中推送数据
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(value);
        condition_variable.notify_one();
    }

    // 从队列中弹出数据
    T pop() {
        std::unique_lock<std::mutex> lock(mutex);
        condition_variable.wait(lock, [this] { return !queue.empty(); });
        T value = queue.front();
        queue.pop();
        return value;
    }
private:
    std::queue<T> queue; 
    mutable std::mutex mutex;
    std::condition_variable condition_variable;
};

template <typename T, typename Compare = std::less<typename std::vector<T>::value_type>>
class ThreadSafePriorityQueue {
public:
    ThreadSafePriorityQueue() = default;

    bool empty() const {
            std::lock_guard<std::mutex> lock(mutex);
            return priorityQueue.empty();
        }

    // 向队列中推送数据
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex);
        priorityQueue.push(value);
        condition_variable.notify_one();
    }

    // 从队列中弹出数据
    T pop() {
        std::unique_lock<std::mutex> lock(mutex);
        condition_variable.wait(lock, [this] { return !priorityQueue.empty(); });
        T value = priorityQueue.front();
        priorityQueue.pop();
        return value;
    }

private:
    std::priority_queue<T, std::vector<T>, Compare> priorityQueue; 
    std::mutex mutex;
    std::condition_variable condition_variable;
};

// 来自SCSN数据使用地址
ThreadSafeQueue<DataPack*> scsnToInfoAnalyQueue; // SCSN-->信息解析模块
ThreadSafeQueue<DataPack*> infoAnalyToClusterManQueue; // 信息解析模块-->集群管理模块
ThreadSafeQueue<DataPack*> infoAnalyToCommProcQueue;  // 信息解析模块-->指令处理模块
ThreadSafeQueue<DataPack*> infoAnalyToDataProcQueue; // 信息解析模块-->数据处理模块
ThreadSafeQueue<DataPack*> infoAnalyToRouteManQueue; // 信息解析模块-->路由管理模块
// 模块返回数据无法使用地址
ThreadSafeQueue<DataPack> otherModuToInfoAnalyQueue; // 集群管理模块、数据处理模块、路由管理模块-->信息解析模块
ThreadSafeQueue<DataPack> commProcToInfoAnalyQueue; // 指令处理模块-->信息解析模块
ThreadSafeQueue<DataPack> subDataToCommProcQueue; // 订阅信息-->指令处理模块(CommProcToInfoAnalyQueue)

std::vector<DataPack> students; // 用于存放结构体的vector,模拟数据

// 定义一个使用 DataPack 类型的 priority_queue，使用 std::less 来进行降序排序,存放来自信息解析模块的优先级数据
ThreadSafePriorityQueue<DataPack*> commProcPriorityQueue;

void acceptAndProduce(ThreadSafeQueue<DataPack*>& scsnToInfoAnalyQueue, std::queue<DataPack*>& ptrQueueSCSN);
void consumeAndDistribute(ThreadSafeQueue<DataPack*>& scsnToInfoAnalyQueue);
std::queue<DataPack*> generateSimulationData();
void judgeAndDistribute(DataPack* ptr);
void infoAnalyListenOther();
void infoAnalyListenCommProc();
void exeInfoAnalyModule();
void commProcListenInfoAnaly();
void commProcListenSubData();
void exeCommProcModule();