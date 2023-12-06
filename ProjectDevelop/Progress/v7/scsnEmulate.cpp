/**
* @file scsnEmulate.cpp
* @author Makaka
* @date 2023-11-08
* @brief 在妙算上运行，运行顺序为先运行，为保证post成功，post函数先停10秒再运行，即需要10秒之内运行CloudThread.py
*        模拟SCSN接收fastapi接口数据并传到无人机端,端口号为8001
*/
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include "nlohmann/json.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include "scsnDataHead.h"

using json = nlohmann::json;
using namespace std;

const int RECEIVEPORT = 5001;   //socket开放端口

/**
* @brief 队列模板,重写pop与push指令,支持定义的队列存取int、string等基本数据类型
* @param None
* @return None
*/
template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;

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
    std::mutex mutex;
    std::condition_variable condition_variable;
};

void postData(int clientSocket, sockaddr_in serverAddr){
    /**
    * @brief 先延时10秒,之后与服务端socket连接,每隔1秒发送一次数据      
    * @param clientSocket 本身socket初始化信息
    * @param serverAddr  服务端IP
    * @return None
    */
    usleep(10000000);
    cout << "Post delay 10 seconds" << endl;
    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        close(clientSocket);
        return ;
    }
    // 持续发送数据
    const char* message = "Hello from client";
    while (true) {
        int bytesSent = send(clientSocket, message, strlen(message), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            break;
        }
        cout << "Sent data: " << message << endl;

        // 添加适当的延时，以控制发送速率
        usleep(10000000); // 休眠 1 秒
    }
}

void receiveData(int serverSocket, sockaddr_in serverAddr, ThreadSafeQueue<std::string>& sharedQueue) {
    /**
    * @brief socket服务端,被连接后若接收到发送来的数据,将其push进shareQueue中
    * @param serverSocket  本身socket初始化信息
    * @param serverAddr   本身IP(本身即服务端)
    * @param sharedQueue  共享队列，与函数consumerFun共享，存放获取的数据（字符串），也可后期处理后再存放进去
    * @return None
    */
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding");
        return ;
    }

    // 监听连接
    if (listen(serverSocket, 5) == -1) {
        perror("Error listening");
        return ;
    }
    // 接受连接
    cout << "Server listening on port 5000..." << endl;
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Error accepting client connection");
        return;
    }
    
    cout << "Client connected" << endl;

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == -1) {
            perror("Error receiving data");
            break;
        }
        if (bytesReceived == 0) {
            cout << "Client disconnected" << endl;
            break;
        }
        cout << "Received: " << buffer << endl;
        sharedQueue.push(buffer);

        // 发送响应
        const char* response = "Hello from server";
        send(clientSocket, response, strlen(response), 0);
    }

    close(clientSocket);
}

struct MyStruct {
    int intValue;
    float floatValue;
    char stringValue[20];
};

void consumerFun(ThreadSafeQueue<std::string>& sharedQueue, int uavSocket, sockaddr_in uavAddr) {
    /**
    * @brief 一直等待shareQueue中的数据,将其取出(无人机指令及参数)，理论上是被处理后的只剩下指令与参数的字符串，转成指定格式传给无人机执行即可
    *        这是阻塞的，直到shareQueue有数据才能被消费，否则一直等待
    *        将队列数据传给无人机
    * @param shareQueue  共享队列，与函数receiveData共享
    * @return None
    */
    // usleep(10000000);
    // cout << "Post delay 10 seconds" << endl;
    // 连接到无人机
    DataPack dataToSend;
    dataToSend.setPackType(0b11);
    dataToSend.setDataSheetIdentificationNum(0b111);
    
    if (connect(uavSocket, (struct sockaddr*)&uavAddr, sizeof(uavAddr)) == -1) {
        perror("Error connecting to server");
        close(uavSocket);
        return ;
    }
    while (true) {
        std::string value = sharedQueue.pop(); 
        int bytesSent = send(uavSocket, &dataToSend, sizeof(dataToSend), 0);
        std::cout << "已发送" << std::endl;
        if (bytesSent == -1) {
            perror("Error sending data");
            break;
        }
        std::cout << "发送数据: " << dataToSend.getPackType() << " Id  : " << dataToSend.getDataSheetIdentificationNum() << std::endl;
        std::cout << "Sent data: " << value << std::endl;
    }
}

int main() {
    /**
    * @brief 主函数，负责创建、启动与管理线程，包括socket的收发两个线程,等待队列中指令发送给无人机线程
    * @param None
    * @return 0
    */
    ThreadSafeQueue<std::string> sharedCommandQueue;
    // 创建 Socket Poster
    int clientSocketPoster = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketPoster == -1) {
        perror("Error creating socket");
        return 1;
    }

    // 设置服务器的 IP 地址和端口号 Poster
    sockaddr_in serverAddrPoster;
    serverAddrPoster.sin_family = AF_INET;
    serverAddrPoster.sin_port = htons(5000); // 替换为服务器的端口号
    serverAddrPoster.sin_addr.s_addr = inet_addr("192.168.10.83"); // 替换为服务器的 IP 地址

    int uavSocketPoster = socket(AF_INET, SOCK_STREAM, 0);
        if (uavSocketPoster == -1) {
            perror("Error creating socket");
            return 1;
        }
    // 设置无人机的 IP 地址和端口号 发送数据
    sockaddr_in uavAddrPoster;
    uavAddrPoster.sin_family = AF_INET;
    uavAddrPoster.sin_port = htons(8001); // 替换为服务器的端口号
    uavAddrPoster.sin_addr.s_addr = inet_addr("192.168.10.83"); // 替换为服务器的 IP 地址

    // 创建Socket
    int serverSocketReceiver = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketReceiver == -1) {
        perror("Error creating socket");
        return 1;
    }

    // 绑定IP地址和端口
    sockaddr_in serverAddrReceiver;
    serverAddrReceiver.sin_family = AF_INET;
    serverAddrReceiver.sin_port = htons(RECEIVEPORT); // 使用端口5001
    serverAddrReceiver.sin_addr.s_addr = INADDR_ANY; // 监听所有网卡上的连接

    thread posterThread(postData, clientSocketPoster, serverAddrPoster);
    thread receiverThread(receiveData, serverSocketReceiver, serverAddrReceiver, std::ref(sharedCommandQueue));
    thread consumerThread(consumerFun, std::ref(sharedCommandQueue), uavSocketPoster, uavAddrPoster);

    // 等待接收线程完成
    receiverThread.join();
    posterThread.join();

    // 关闭 Socket
    close(clientSocketPoster);
    close(serverSocketReceiver);

    return 0;
}
