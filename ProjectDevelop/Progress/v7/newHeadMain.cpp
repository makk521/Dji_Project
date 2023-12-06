/**
* @file newHeadMain.cpp
* @author Makaka
* @date 2023-12-06
* @brief 新结构体的main函数
*/
#include "dataHead.h"
#include <sys/socket.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <netinet/in.h>

void receiveData() {
    // 创建套接字
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        return;
    }

    // 准备地址结构体
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8001);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定地址
    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        perror("Error binding");
        close(serverSocket);
        return;
    }

    // 监听连接
    if (listen(serverSocket, 5) == -1) {
        perror("Error listening");
        close(serverSocket);
        return;
    }

    std::cout << "等待客户端连接..." << std::endl;

    // 接受连接
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);

    if (clientSocket == -1) {
        perror("Error accepting client connection");
        close(serverSocket);
        return;
    }

    std::cout << "客户端连接成功" << std::endl;

    // 接收和打印数据
    char buffer[1024];
    while (true) {
        // std::memset(buffer, 0, sizeof(buffer));
        // int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        DataPack receivedData;
        int bytesReceived = recv(clientSocket, &receivedData, sizeof(receivedData), 0);

        if (bytesReceived == -1) {
            perror("Error receiving data");
            break;
        }

        if (bytesReceived == 0) {
            std::cout << "客户端断开连接" << std::endl;
            break;
        }

        // std::cout << "接收到的数据: " << buffer << std::endl;
        std::cout << "接收到的数据: " << receivedData.getPackType() << "Id  : " << receivedData.getDataSheetIdentificationNum() << std::endl;
    }

    // 关闭套接字
    close(clientSocket);
    close(serverSocket);
}

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

    std::thread receiveDataThread(receiveData);
    receiveDataThread.join();
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
        DataPack makaka;
        makaka.setPackType(i/3);
        makaka.setDataSheetIdentificationNum(20 + i);
        students.push_back(makaka);
        std::cout << "生成测试元素：" << "PackType:" << makaka.getPackType() << "  DataSheetIdentificationNum :" << makaka.getDataSheetIdentificationNum() << std::endl;
    }

    std::queue<DataPack*> ptrQueueSCSN;
    for (auto& makaka : students) {
        // 将每个结构体的指针存入ptrQueueSCSN
        DataPack* ptr = &makaka;  // 获取每个元素的地址
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
    switch ((*ptr).getPackType()) {
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
