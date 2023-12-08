/**
* @file dataHead.h
* @author Makaka
* @date 2023-12-06
* @brief 新结构体头文件
*/
/**
* @file datahead.cpp
* @author Makaka
* @date 2023-12-05
* @brief 结构体的函数与定义
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue> 

struct DataPackHeader
{
    uint32_t firstLine; // 包类型(7b) + 信道(3b) + 优先级(6b) + 模块ID(4b) + 包长度(12b)
    uint32_t secondLine; // 包偏移量(12b) + 包序列号(20b)
    uint16_t dataSheetIdentificationNum; // 数据片标识号
    uint16_t targetId; // 目标ID
    uint16_t sourceId; // 源ID
    uint16_t nextHopId; // 下一跳ID
    uint16_t singleHopSourceId; // 单跳源ID
    uint16_t checkSum; // 校验和
    uint32_t validTime; // 有效时间
    uint32_t lastLine;  // 集群ID + 备用字段
};

struct DataPack {
    DataPackHeader header; // 包头
    char* payload; // 数据

    bool operator<(const DataPack& other) const {
        return header.checkSum > other.header.checkSum;
    }

    uint32_t getPackType() {
        uint32_t mask = 0b1111111; 
        uint32_t result = header.firstLine >> 25; 
        return result & mask; 
    }
    void setPackType(uint32_t value) {
        uint32_t mask = 0b1111111 << 25; 
        header.firstLine = (header.firstLine & ~mask) | ((value & 0b1111111) << 25); 
    }

    uint32_t getChannel() {
        uint32_t mask = 0b111; 
        uint32_t result = header.firstLine >> 22; 
        return result & mask; 
    }
    void setChannel(uint32_t value) {
        uint32_t mask = 0b111 << 22;  
        header.firstLine = (header.firstLine & ~mask) | ((value & 0b111) << 22);  
    }

    uint32_t getPacketPriority() {
        uint32_t mask = 0b111111;  
        uint32_t result = header.firstLine >> 16;  
        return result & mask; 
    }
    void setPacketPriority(uint32_t value) {
        uint32_t mask = 0b111111 << 16;  
        header.firstLine = (header.firstLine & ~mask) | ((value & 0b111111) << 16);  
    }

    uint32_t getModuleId() {
        uint32_t mask = 0b1111; 
        uint32_t result = header.firstLine >> 12;  
        return result & mask;  
    }
    void setModuleId(uint32_t value) {
        uint32_t mask = 0b1111 << 12; 
        header.firstLine = (header.firstLine & ~mask) | ((value & 0b1111) << 12); 
    }

    uint32_t getPackLength() {
        uint32_t mask = 0b111111111111; 
        uint32_t result = header.firstLine;  
        return result & mask; 
    }
    void setPackLength(uint32_t value) {
        uint32_t mask = 0b111111111111;  
        header.firstLine = (header.firstLine & ~mask) | (value & 0b111111111111); 
    }

    uint32_t getPackOffset() {
        uint32_t mask = 0b111111111111; 
        uint32_t result = header.secondLine >> 20; 
        return result & mask; 
    }
    void setPackOffset(uint32_t value) {
        uint32_t mask = 0b111111111111 << 20; 
        header.secondLine = (header.secondLine & ~mask) | ((value & 0b111111111111) << 20);
    }

    uint32_t getPackSequenceNum() {
        uint32_t mask = 0b11111111111111111111; 
        uint32_t result = header.secondLine; 
        return result & mask; 
    }
    void setPackSequenceNum(uint32_t value) {
        uint32_t mask = 0b11111111111111111111; 
        header.secondLine = (header.secondLine & ~mask) | (value & 0b11111111111111111111); 
    }

    uint16_t getDataSheetIdentificationNum() {
        return header.dataSheetIdentificationNum;
    }
    void setDataSheetIdentificationNum(uint16_t value) {
        header.dataSheetIdentificationNum = value;
    }

    uint16_t getTargetId() {
        return header.targetId;
    }
    void setTargetId(uint16_t value) {
        header.targetId = value;
    }

    uint16_t getSourceId() {
        return header.sourceId;
    }
    void setSourceId(uint16_t value) {
        header.sourceId = value;
    }

    uint16_t getNextHopId() {
        return header.nextHopId;
    }
    void setNextHopId(uint16_t value) {
        header.nextHopId = value;
    }

    uint16_t getSingleHopSourceId() {
        return header.singleHopSourceId;
    }
    void setSingleHopSourceId(uint16_t value) {
        header.singleHopSourceId = value;
    }

    uint16_t getCheckSum() {
        return header.checkSum;
    }
    void setCheckSum(uint16_t value) {
        header.checkSum = value;
    }

    uint32_t getValidTime() {
        return header.validTime;
    }
    void setValidTime(uint32_t value) {
        header.validTime = value;
    }

    uint32_t getClusterId() {
        uint32_t mask = 0b11111111;  
        uint32_t result = header.lastLine >> 8; 
        return result & mask;  
    }
    void setClusterId(uint32_t value) {
        uint32_t mask = 0b11111111 << 8;  
        header.lastLine = (header.lastLine & ~mask) | ((value & 0b11111111) << 8); 
    }
};

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

// 来自SCSN数据
ThreadSafeQueue<DataPack> scsnQueue;
ThreadSafeQueue<DataPack> scsnToInfoAnalyQueue; // SCSN-->信息解析模块
ThreadSafeQueue<DataPack> infoAnalyToClusterManQueue; // 信息解析模块-->集群管理模块
ThreadSafeQueue<DataPack> infoAnalyToCommProcQueue;  // 信息解析模块-->指令处理模块
ThreadSafeQueue<DataPack> infoAnalyToDataProcQueue; // 信息解析模块-->数据处理模块
ThreadSafeQueue<DataPack> infoAnalyToRouteManQueue; // 信息解析模块-->路由管理模块
// 模块返回数据无法使用地址
ThreadSafeQueue<DataPack> otherModuToInfoAnalyQueue; // 集群管理模块、数据处理模块、路由管理模块-->信息解析模块
ThreadSafeQueue<DataPack> commProcToInfoAnalyQueue; // 指令处理模块-->信息解析模块
ThreadSafeQueue<DataPack> subDataToCommProcQueue; // 订阅信息-->指令处理模块(CommProcToInfoAnalyQueue)

std::vector<DataPack> students; // 用于存放结构体的vector,模拟数据

// 定义一个使用 DataPack 类型的 priority_queue，使用 std::less 来进行降序排序,存放来自信息解析模块的优先级数据
ThreadSafePriorityQueue<DataPack> commProcPriorityQueue;

void acceptAndProduce(ThreadSafeQueue<DataPack>& scsnToInfoAnalyQueue, std::queue<DataPack>& ptrQueueSCSN);
void consumeAndDistribute(ThreadSafeQueue<DataPack>& scsnToInfoAnalyQueue);
std::queue<DataPack> generateSimulationData();
void judgeAndDistribute(DataPack value);
void infoAnalyListenOther();
void infoAnalyListenCommProc();
void commProcListenInfoAnaly();
void commProcListenSubData();
void infoAnalyListenscsn(ThreadSafeQueue<DataPack>& scsnQueue, ThreadSafeQueue<DataPack>& scsnToInfoAnalyQueue);