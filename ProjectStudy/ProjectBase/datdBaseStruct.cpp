/**
* @file datdBaseStruct.cpp
* @author Makaka
* @date 2023-11-20
* @brief c++11实现线程安全队列,生成模拟数据并经信息解析模块分发给各个模块。
*/
#include "dataBaseStruct.h"

int main() {
    std::queue<DataPack*> ptrQueueSCSN;
    ptrQueueSCSN = generateSimulationData();

    // 信息解析模块启动
    std::thread infoAnalyListenscsnThread(acceptAndProduce, std::ref(scsnToInfoAnalyQueue), std::ref(ptrQueueSCSN));
    std::thread infoAnalydistributeThread(consumeAndDistribute, std::ref(scsnToInfoAnalyQueue));
    std::thread infoAnalyListenOtherThread(infoAnalyListenOther);
    std::thread infoAnalyListenCommProcThread(infoAnalyListenCommProc);
    // 指令处理模块启动
    std::thread commProcListenInfoAnalyThread(commProcListenInfoAnaly);
    std::thread commProcListenSubDataThread(commProcListenSubData);

    infoAnalyListenscsnThread.join();
    infoAnalydistributeThread.join();
    infoAnalyListenOtherThread.join();
    infoAnalyListenCommProcThread.join();
    commProcListenInfoAnalyThread.join();
    commProcListenSubDataThread.join();

    return 0;
}

std::queue<DataPack*> generateSimulationData(){
    /**
    * @brief 生成模拟数据,即从SCSN到信息解析模块的队列（全是指针）
    * @param None
    * @return ptrQueueSCSN 放入指针的queue，模拟SCSN发送给信息解析模块的数据
    */
    for (int i = 0; i < 12; ++i) {
        // 创建10个结构体并存入students
        DataPack student;
        student.header.packetType = i/3;
        student.header.packetIdentificationNum = 20 + i;
        students.push_back(student);
        std::cout << "生成测试元素：" << "packetType:" << student.header.packetType << "  packetIdentificationNum :" << student.header.packetIdentificationNum << std::endl;
    }

    std::queue<DataPack*> ptrQueueSCSN;
    for (auto& student : students) {
        // 将每个结构体的指针存入ptrQueueSCSN
        DataPack* ptr = &student;  // 获取每个元素的地址
        ptrQueueSCSN.push(ptr);
        // std::cout << "已存入指针 :" << ptr << std::endl;
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
            // std::cout << "将元素从SCSN取出放入信息解析模块: " << " packetIdentificationNum: " << ptr->header.packetIdentificationNum << std::endl;
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
            // std::cout << "Consumed: " << "packetType: " << ptr->header.packetType << ", packetIdentificationNum: " << ptr->header.packetIdentificationNum << std::endl;
        }
    }
}

void judgeAndDistribute(DataPack* ptr){
    /**
    * @brief 判断数据类型并分发给其他模块
    * @param ptr 需要分发数据的指针
    * @return None
    */
    switch (ptr->header.packetType) {
        case 0:
            infoAnalyToClusterManQueue.push(ptr);
            std::cout << "已分发到集群管理模块" << std::endl;
            break;
        case 1:
            infoAnalyToCommProcQueue.push(ptr);
            std::cout << "已分发到指令处理模块" << std::endl;
            break;
        case 2:
            infoAnalyToDataProcQueue.push(ptr);
            std::cout << "已分发到数据处理模块" << std::endl;
            break;
        case 3:
            infoAnalyToRouteManQueue.push(ptr);
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
            std::cout << "信息解析模块收到集群管理模块等三个模块的返回数据" << std::endl;
        }
    }
}

void infoAnalyListenCommProc(){
    /**
    * @brief 信息解析模块监听指令处理模块返回值队列commProcToInfoAnalyQueue
    * @param None
    * @return None
    */
    while (true) {
        if (!commProcToInfoAnalyQueue.empty()) {
            DataPack ptr = commProcToInfoAnalyQueue.pop();
            std::cout << "信息解析模块收到指令处理模块的返回数据" << std::endl;
        }
    }
}

void commProcListenInfoAnaly(){
    /**
    * @brief 指令处理模块监听信息解析模块返回值队列commProcToInfoAnalyQueue并将其放入优先级队列commProcPriorityQueue
    * @param None
    * @return None
    */
    while (true) {
        // std::cout << "Execute" << std::endl;
        if (!infoAnalyToCommProcQueue.empty()) {
            DataPack* ptr = infoAnalyToCommProcQueue.pop();
            commProcPriorityQueue.push(ptr);
            std::cout << "指令处理模块收到信息解析模块的指令并将其传给无人机" << std::endl;
        }
    }
}

void commProcListenSubData(){
    /**
    * @brief 指令处理模块监听订阅信息队列subDataToCommProcQueue并将其放入优先级队列commProcPriorityQueue
    * @param None
    * @return None
    */
    while (true) {
        if (!subDataToCommProcQueue.empty()) {
            DataPack ptr = subDataToCommProcQueue.pop();
            commProcToInfoAnalyQueue.push(ptr);
            std::cout << "指令处理模块收到订阅信息并将其传给信息解析模块" << std::endl;
        }
    }
}
