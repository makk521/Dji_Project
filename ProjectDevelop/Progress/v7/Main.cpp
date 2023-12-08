/**
* @file newHeadMain.cpp
* @author Makaka
* @date 2023-12-06
* @brief 新结构体的main函数
*/
#include "dataPack.h"
#include <sys/cdefs.h>
#include <sys/socket.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <netinet/in.h>

void receiveData() {
    /**
    * @brief 接收数据函数，并将数据存入scsnQueue
    * @param None
    * @return None
    */
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

        std::cout << "接收到的数据:PackType: " << receivedData.getPackType() << "  Id  : " << receivedData.getDataSheetIdentificationNum() << std::endl;
        scsnQueue.push(receivedData);
    }

    // 关闭套接字
    close(clientSocket);
    close(serverSocket);
}

int main() {
    // 信息解析模块启动
    std::thread infoAnalydistributeThread(consumeAndDistribute, std::ref(scsnToInfoAnalyQueue));
    std::thread infoAnalyListenOtherThread(infoAnalyListenOther);
    std::thread infoAnalyListenCommProcThread(infoAnalyListenCommProc);
    // 指令处理模块启动
    std::thread commProcListenInfoAnalyThread(commProcListenInfoAnaly);
    std::thread commProcListenSubDataThread(commProcListenSubData);

    std::thread receiveDataThread(receiveData);
    std::thread infoAnalyListenscsnThread(infoAnalyListenscsn, std::ref(scsnQueue), std::ref(scsnToInfoAnalyQueue));
    
    infoAnalydistributeThread.join();
    infoAnalyListenOtherThread.join();
    infoAnalyListenCommProcThread.join();
    commProcListenInfoAnalyThread.join();
    commProcListenSubDataThread.join();
    receiveDataThread.join();
    infoAnalyListenscsnThread.join();

    return 0;
}

void acceptAndProduce(ThreadSafeQueue<DataPack>& scsnToInfoAnalyQueue,std::queue<DataPack>& scsnQueue) { 
    /**
    * @brief 生产者函数，与SCSN交互，将scsnQueue中的数据（结构体）取出放入scsnToInfoAnalyQueue，即信息解析模块中
    * @param scsnToInfoAnalyQueue 暂存队列放入信息解析模块用于后续处理  scsnQueue SCSN中的数据源
    * @return None
    */
    while (true) {
        if (!scsnQueue.empty()) {
            auto value = scsnQueue.front();
            scsnToInfoAnalyQueue.push(value);
            scsnQueue.pop();
            // std::cout << "将元素从SCSN取出放入信息解析模块: " << " packetIdentificationNum: " << ptr->header.packetIdentificationNum << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
    }
}

void infoAnalyListenscsn(ThreadSafeQueue<DataPack>& scsnQueue, ThreadSafeQueue<DataPack>& scsnToInfoAnalyQueue) {
    /**
    * @brief 信息解析模块监听SCSN返回值队列scsnQueue并存入scsnToInfoAnalyQueue
    * @param scsnToInfoAnalyQueue 暂存队列
    * @return None
    */
    while (true) {
        if (!scsnQueue.empty()) {
            DataPack value = scsnQueue.pop();
            scsnToInfoAnalyQueue.push(value);
            std::cout << "信息解析模块收到SCSN的数据: " << std::endl;
        }
    }
}

void consumeAndDistribute(ThreadSafeQueue<DataPack>& scsnToInfoAnalyQueue) { 
    /**
    * @brief 消费者函数，将scsnToInfoAnalyQueue中的数据（结构体）取出判断后分发给其他模块
    * @param scsnToInfoAnalyQueue 暂存队列
    * @return None
    */
    while (true) {
        if (!scsnToInfoAnalyQueue.empty()) {
            DataPack value = scsnToInfoAnalyQueue.pop();
            judgeAndDistribute(value); // 判断并分发指针到各个模块
            // std::cout << "Consumed: " << "packetType: " << ptr->header.packetType << ", packetIdentificationNum: " << ptr->header.packetIdentificationNum << std::endl;
        }
    }
}

void judgeAndDistribute(DataPack value){
    /**
    * @brief 判断数据类型并分发给其他模块
    * @param value 需要分发的数据包
    * @return None
    */
    switch (value.getPackType()) {
        case 0:
            infoAnalyToClusterManQueue.push(value);
            std::cout << "已分发到集群管理模块" << std::endl;
            break;
        case 1:
            infoAnalyToCommProcQueue.push(value);
            std::cout << "已分发到指令处理模块" << std::endl;
            break;
        case 2:
            infoAnalyToDataProcQueue.push(value);
            std::cout << "已分发到数据处理模块" << std::endl;
            break;
        case 3:
            infoAnalyToRouteManQueue.push(value);
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
            DataPack value = infoAnalyToCommProcQueue.pop();
            commProcPriorityQueue.push(value);
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
