/**
* @file datdBaseStruct.cpp
* @author Makaka
* @date 2023-11-20
* @brief c++11实现线程安全队列,生成模拟数据并经信息解析模块分发给各个模块。
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue> 

struct DataPack {
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
    uint8_t* payload; // 数据
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

void acceptAndProduce(ThreadSafeQueue<DataPack*>& scsnToInfoAnalyQueue, std::queue<DataPack*>& ptrQueueSCSN);
void consumeAndDistribute(ThreadSafeQueue<DataPack*>& scsnToInfoAnalyQueue);
std::queue<DataPack*> generateSimulationData();
void judgeAndDistribute(DataPack* ptr);
void infoAnalyListenOther();
void infoAnalyListenCommProc();

// 来自SCSN数据使用地址
ThreadSafeQueue<DataPack*> scsnToInfoAnalyQueue; // SCSN-->信息解析模块
ThreadSafeQueue<DataPack*> InfoAnalyToClusterManQueue; // 信息解析模块-->集群管理模块
ThreadSafeQueue<DataPack*> InfoAnalyToCommProcQueue;  // 信息解析模块-->指令处理模块
ThreadSafeQueue<DataPack*> InfoAnalyToDataProcQueue; // 信息解析模块-->数据处理模块
ThreadSafeQueue<DataPack*> InfoAnalyToRouteManQueue; // 信息解析模块-->路由管理模块
// 模块返回数据无法使用地址
ThreadSafeQueue<DataPack> otherModuToInfoAnalyQueue; // 集群管理模块、数据处理模块、路由管理模块-->信息解析模块
ThreadSafeQueue<DataPack> CommProcToInfoAnalyQueue; // 指令处理模块-->信息解析模块

std::vector<DataPack> students; // 用于存放结构体的vector,模拟数据

int main() {
    std::queue<DataPack*> ptrQueueSCSN;
    ptrQueueSCSN = generateSimulationData();

    // 将ptrQueueSCSN中的指针存入scsnToInfoAnalyQueue
    std::thread producerThread(acceptAndProduce, std::ref(scsnToInfoAnalyQueue), std::ref(ptrQueueSCSN));
    std::thread consumerThread(consumeAndDistribute, std::ref(scsnToInfoAnalyQueue));

    producerThread.join();
    consumerThread.join();

    return 0;
}

std::queue<DataPack*> generateSimulationData(){
    /**
    * @brief 生成模拟数据,即从SCSN到信息解析模块的队列（全是指针）
    * @param None
    * @return ptrQueueSCSN 放入指针的queue，模拟SCSN发送给信息解析模块的数据
    */
    for (int i = 0; i < 10; ++i) {
        // 创建10个结构体并存入students
        DataPack student;
        student.packetType = i;
        student.packetIdentificationNum = 20 + i;
        students.push_back(student);
    }

    std::queue<DataPack*> ptrQueueSCSN;
    for (auto& student : students) {
        // 将每个结构体的指针存入ptrQueueSCSN
        DataPack* ptr = &student;  // 获取每个元素的地址
        ptrQueueSCSN.push(ptr);
        std::cout << "已存入指针 :" << ptr << std::endl;
    }
    return ptrQueueSCSN;
}

void acceptAndProduce(ThreadSafeQueue<DataPack*>& scsnToInfoAnalyQueue,std::queue<DataPack*>& ptrQueueSCSN) { 
    /**
    * @brief 生产者函数，与SCSN交互，将ptrQueueSCSN中的数据（指针）取出放入scsnToInfoAnalyQueue，即信息解析模块中
    * @param scsnToInfoAnalyQueue 暂存队列放入信息解析模块用于后续处理  ptrQueueSCSN SCSN中的数据源
    * @return None
    */
    while (true) {
        if (!ptrQueueSCSN.empty()) {
            auto& ptr = ptrQueueSCSN.front();
            scsnToInfoAnalyQueue.push(ptr);
            ptrQueueSCSN.pop();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
    }
}

void consumeAndDistribute(ThreadSafeQueue<DataPack*>& scsnToInfoAnalyQueue) { 
    /**
    * @brief 消费者函数，将scsnToInfoAnalyQueue中的数据（指针）取出判断后分发给其他模块
    * @param scsnToInfoAnalyQueue 暂存队列
    * @return None
    */
    while (true) {
        if (!scsnToInfoAnalyQueue.empty()) {
            DataPack* ptr = scsnToInfoAnalyQueue.pop();
            judgeAndDistribute(ptr); // 判断并分发指针到各个模块
            std::cout << "Consumed: " << "Name: " << ptr->packetType << ", Age: " << ptr->packetIdentificationNum << std::endl;
        }
    }
}

void judgeAndDistribute(DataPack* ptr){
    /**
    * @brief 判断数据类型并分发给其他模块
    * @param ptr 需要分发数据的指针
    * @return None
    */
    switch (ptr->packetType) {
        case 0:
            InfoAnalyToClusterManQueue.push(ptr);
            std::cout << "已分发到集群管理模块" << std::endl;
            break;
        case 1:
            InfoAnalyToCommProcQueue.push(ptr);
            std::cout << "已分发到指令处理模块" << std::endl;
            break;
        case 2:
            InfoAnalyToDataProcQueue.push(ptr);
            std::cout << "已分发到数据处理模块" << std::endl;
            break;
        case 3:
            InfoAnalyToRouteManQueue.push(ptr);
            std::cout << "已分发到路由管理模块" << std::endl;
            break;
        default:
            break;
    }
}

void infoAnalyListenOther(){
    /**
    * @brief 信息解析模块监听集群管理模块、数据处理模块、路由管理模块返回值队列otherModuToInfoAnalyQueue
    * @param None
    * @return None
    */
    while (true) {
        if (!otherModuToInfoAnalyQueue.empty()) {
            DataPack ptr = otherModuToInfoAnalyQueue.pop();
            std::cout << "信息解析模块收到集群管理模块的数据" << std::endl;
        }
    }
}

void infoAnalyListenCommProc(){
    /**
    * @brief 信息解析模块监听指令处理模块返回值队列CommProcToInfoAnalyQueue
    * @param None
    * @return None
    */
    while (true) {
        if (!CommProcToInfoAnalyQueue.empty()) {
            DataPack ptr = CommProcToInfoAnalyQueue.pop();
            std::cout << "信息解析模块收到指令处理模块的数据" << std::endl;
        }
    }
}
