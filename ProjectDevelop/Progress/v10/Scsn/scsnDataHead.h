/**
* @file scsnDataHead.h
* @author Makaka
* @date 2023-12-06
* @brief scsn模拟器调用的包头
*/
#ifndef DATAPACKDEFINE
#define DATAPACKDEFINE
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue> 
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

    void coutDataPackHeader() {
        std::cout << "PackType: " << getPackType() << std::endl;
        std::cout << "Channel: " << getChannel() << std::endl;
        std::cout << "PacketPriority: " << getPacketPriority() << std::endl;
        std::cout << "ModuleId: " << getModuleId() << std::endl;
        std::cout << "PackLength: " << getPackLength() << std::endl;
        std::cout << "PackOffset: " << getPackOffset() << std::endl;
        std::cout << "PackSequenceNum: " << getPackSequenceNum() << std::endl;
        std::cout << "DataSheetIdentificationNum: " << getDataSheetIdentificationNum() << std::endl;
        std::cout << "TargetId: " << getTargetId() << std::endl;
        std::cout << "SourceId: " << getSourceId() << std::endl;
        std::cout << "NextHopId: " << getNextHopId() << std::endl;
        std::cout << "SingleHopSourceId: " << getSingleHopSourceId() << std::endl;
        std::cout << "CheckSum: " << getCheckSum() << std::endl;
        std::cout << "ValidTime: " << getValidTime() << std::endl;
        std::cout << "ClusterId: " << getClusterId() << std::endl;
    }
};

/**
* @brief 队列模板,重写pop与push指令,支持定义的队列存取int、string等基本数据类型
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
    std::queue<T> queue;  // Use std::queue to store data
    mutable std::mutex mutex;
    std::condition_variable condition_variable;
};

void RedisInit(std::string IP, int PORT, std::string PASSWORD); // Redis使用初始化
void subinfo2Redis(std::string IP, int PORT, std::string PASSWORD, std::string PostData); // 将postData传入数据库
void postToPythonData(int clientSocket, sockaddr_in serverAddr); // 发送数据
void receivePythonData(int serverSocket, sockaddr_in serverAddr, ThreadSafeQueue<std::string>& sharedCommandQueue); // 接受fastapi数据并放入sharedCommandQueue
void consumerFun(ThreadSafeQueue<std::string>& sendDataQueue, int uavSocket, sockaddr_in uavAddr); // 监听sendDataQueue并将其中数据打包发送给无人机
// 接收来自无人机的数据，并根据type判断下一步走哪里
void receiveCData(int HOST, ThreadSafeQueue<DataPack>& mobilityQueue, ThreadSafeQueue<DataPack>& uavSubQueue, ThreadSafeQueue<DataPack>& groupDirectiveExecutionQueue);
void listenMobilityQueue(ThreadSafeQueue<DataPack>& mobilityQueue); // 监听移动性管理模块数据
void listenUavDataQueue(ThreadSafeQueue<DataPack>& uavSubQueue); // 监听无人机订阅返回数据
void connectWithRetry(int clientSocket, sockaddr_in serverAddr, int maxRetries, int retryDelaySeconds); // 超时重连服务端
void printMessage(const std::string& message); // 线程安全的打印
#endif