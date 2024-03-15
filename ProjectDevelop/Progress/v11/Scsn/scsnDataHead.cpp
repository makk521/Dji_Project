#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include "json.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <curl/curl.h>
#include "Mobility.hpp"
#include "post.hpp"
#include "scsnDataHead.h"

using json = nlohmann::json;

extern const int RECEIVEFROMPYTHONPORT = 5001;   // 对python的开放端口
extern const int RECEIVEFROMCPORT = 5002; // 对c++的开放端口
int uavPackType[] = {0, 1, 2, 3}; //无人机返回数据包的type
extern const string CALLBACKURL = "http://192.168.20.122:8080/ucs/uav/commandCallback";
extern const string OFFLINEURL = "http://192.168.10.230:8083/mock/701/ucs/uav/deactivate";
extern const std::string REDISIP = "172.21.2.1";
extern const int REDISPORT = 6379;
extern const std::string REDISPASSWORD = "Ustc1958@2023";
extern const int MAXRETRIES = 10; // 最大重连次数
extern const int RETRYDELAYSECONDS = 5;// 每次重连间隔时间(秒)
std::mutex COUTMUTEX; // 打印锁
// el::Configurations conf;
// 收到来自fastapi传来的字符串
ThreadSafeQueue<std::string> sharedCommandQueue;
// // 处理后放入这个队列,如：{
//             "uid": "1",
//             "action": "move", //指点飞行
//             "sParams": {"x":11, "y":12, "z":32},  //float, 当前位置的参数；服务端用
//             "dParams": {"x": 12, "y": 22, "z": 33, "height":21},  //float，目标位置参数；无人机用
//             "commandNum": "cmdNum:003:move:1704444270941",
//             "timeStamp": "1704444270941s",
//             "priority": "1",
//         }
ThreadSafeQueue<std::string> sendDataQueue; // 执行情况向无人机端发送数据
ThreadSafeQueue<DataPack> groupDirectiveExecutionQueue; // 编队执行情况返回


void RedisInit(std::string IP, int PORT, std::string PASSWORD){
    /**
    * @brief Redis初始化，若无则无法在线程中直接插入成功
    * @param Redis的IP、端口与密码
    * @return None
    */
    RedisConnect::Setup(IP, PORT, PASSWORD);
    shared_ptr<RedisConnect> redis = RedisConnect::Instance();
    redis->select(11);
}

void subinfo2Redis(std::string IP, int PORT, std::string PASSWORD, std::string PostData) {
    /**
    * @brief 将postData传入数据库，地址为ma_testUAVIP:
    * @param PostData发送数据
    * @return None
    */
    static int UAVIP = 1;
    RedisConnect::Setup(IP, PORT, PASSWORD);
    shared_ptr<RedisConnect> redis = RedisConnect::Instance();
    redis->select(11);

    // 在这里执行你的任务
    redis->set("uav:" + to_string(UAVIP) +":realtime", PostData);
    // UAVIP++;
    // cout << "插入成功一次" << endl;
}

void postToPythonData(int clientSocket, sockaddr_in serverAddr){
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

void printMessage(const std::string& message) {
    // 使用互斥锁保护 std::cout
    std::lock_guard<std::mutex> lock(COUTMUTEX);
    
    // 打印信息
    std::cout << message << std::endl;
}

void receivePythonData(int serverSocket, sockaddr_in serverAddr, ThreadSafeQueue<std::string>& sharedCommandQueue) {
    /**
    * @brief socket服务端,被连接后若接收到发送来的数据,将其push进shareQueue中
    * @param serverSocket  本身socket初始化信息
    * @param serverAddr   本身IP(本身即服务端)
    * @param sharedCommandQueue  共享队列，与函数consumerFun共享，存放获取的数据（字符串），也可后期处理后再存放进去
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
    // 等待连接
    std::string message = "开放端口";
    message +=  std::to_string(RECEIVEFROMPYTHONPORT);
    message += "等待客户端(fastapi)连接...";
    printMessage(message);

    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Error accepting client connection");
        return;
    }
    
    printMessage("fastapi连接成功");

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
        cout << "receivePythonThread received: " << buffer << endl;
        LOG(INFO) << "SCSN收到来自fastapi的数据：" << buffer <<endl;
        sharedCommandQueue.push(buffer);

        // 发送响应
        const char* response = "Hello from server";
        send(clientSocket, response, strlen(response), 0);
    }

    close(clientSocket);
}

void connectWithRetry(int clientSocket, sockaddr_in serverAddr, int maxRetries, int retryDelaySeconds) {
    /**
    * @brief 尝试多次连接服务端   
    * @param maxRetries 最大尝试次数
    * @param retryDelaySeconds  每次尝试间隔
    * @return None
    */
    for (int attempt = 1; attempt <= maxRetries; ++attempt) {
        std::string message = "Attempting to connect Server (Attempt ";
        message += std::to_string(attempt);
        printMessage(message);

        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
            std::cout << "Connected to server" << std::endl;
            return;
        }

        // 连接失败，等待一段时间再次尝试
        std::this_thread::sleep_for(std::chrono::seconds(retryDelaySeconds));
    }

    std::cout << "Failed to connect after multiple attempts" << std::endl;
}

void consumerFun(ThreadSafeQueue<std::string>& sendDataQueue, int uavSocket, sockaddr_in uavAddr) {
    /**
    * @brief 一直等待shareQueue中的数据,将其取出(无人机指令及参数)，理论上是被处理后的只剩下指令与参数的字符串，转成指定格式传给无人机执行即可
    *        这是阻塞的，直到shareQueue有数据才能被消费，否则一直等待
    *        将队列数据传给无人机
    *        多无人机后，马际清将前端的数据处理后放到sendDataQueue(sendDataQueue)，该函数监听该队列并封包发送给无人机
    * @param shareQueue  共享队列，与函数receivePythonData共享
    * @return None
    */
    // usleep(10000000);
    // cout << "Post delay 10 seconds" << endl;
    // 连接到无人机
    DataPack dataToSendPacK;
    // dataToSendPacK.setPackType(0b1);
    dataToSendPacK.setChannel(0b1);
    dataToSendPacK.setPacketPriority(0b1);
    dataToSendPacK.setModuleId(0b1111);
    dataToSendPacK.setPackLength(0b1);
    dataToSendPacK.setPackOffset(0b1);
    dataToSendPacK.setPackSequenceNum(0b1);
    dataToSendPacK.setPackSequenceNum(0b1);
    dataToSendPacK.setDataSheetIdentificationNum(0b1);
    dataToSendPacK.setTargetId(0b1);
    dataToSendPacK.setSourceId(0b1);
    dataToSendPacK.setNextHopId(0b1);
    dataToSendPacK.setSingleHopSourceId(0b1);
    dataToSendPacK.setCheckSum(0b1);
    dataToSendPacK.setClusterId(0b1);

    char *dataToSend = static_cast<char*>(malloc(2000));
 
    // if (connect(uavSocket, (struct sockaddr*)&uavAddr, sizeof(uavAddr)) == -1) {
    //     perror("Error connecting to server");
    //     close(uavSocket);
    //     return ;
    // }
    // 连接无人机端(服务端)
    int maxConnectionRetries = 5, retryDelaySeconds = 5;
    connectWithRetry(uavSocket, uavAddr, MAXRETRIES, RETRYDELAYSECONDS);

    while (true) {
        std::string value = sendDataQueue.pop(); // 取出未处理的字符串{'uid': '2', 'uavType': '', 'action': 'takeoff', 'params': [], 'commandNum': ''}
        LOG(INFO) << "SCSN模块从sendDataQueue取出数据：" << value;
        //std::cout << "从sendDataQueue取出" << value << std::endl;
        
        std::replace(value.begin(), value.end(), '\'', '\"'); // 将'转"使得可以转为json格式
        json valueJson = json::parse(value);
        json payloadJson; // 将value中有用的数据存到string中
        payloadJson["uid"] = valueJson["uid"];
        payloadJson["action"] = valueJson["action"];
        payloadJson["sParams"] = valueJson["sparams"];
        payloadJson["dParams"] = valueJson["dparams"];
        payloadJson["commandNum"] = valueJson["commandNum"];
        payloadJson["priority"] = valueJson["priority"];
        payloadJson["timeStamp"] = valueJson["timeStamp"];
        payloadJson["packType"] = valueJson["packType"];
        std::string payloadString = payloadJson.dump();
        dataToSendPacK.setPackType(valueJson["packType"]);
        dataToSendPacK.setPackLength(payloadString.length()); // 设置长度
        
        memcpy(dataToSend, &dataToSendPacK.header, sizeof(dataToSendPacK.header)); // 将头部复制到dataToSend中（在此之前封装完成）
        DataPack temp; // 暂时的，展示发送的数据是否符合要求
        memcpy(&temp.header, dataToSend, sizeof(temp.header)); // 将发送的数据(包头部分)复制一份，检查是否发送正确
        // 将指令信息memcpy到dataToSend中
        memcpy(dataToSend + sizeof(dataToSendPacK.header), payloadString.c_str(), payloadString.length()); // 长度注意改！！！
        
        // int bytesSent = send(uavSocket, &dataToSendPacK, sizeof(dataToSendPacK), 0);
        int bytesSent = send(uavSocket, dataToSend, 2000, 0);  // int socket, const void *buffer, size_t length, int flags
        std::cout << "已发送" << std::endl;
        if (bytesSent == -1) {
            perror("Error sending data");
            break;
        }
        // temp.coutDataPackHeader();
        //std::cout << "发送数据Id" << temp.getDataSheetIdentificationNum() << std::endl;
        // std::cout << "发送数据: " << dataToSendPacK.getPackType() << " Id  : " << dataToSendPacK.getDataSheetIdentificationNum() << std::endl;
        //std::cout << "发送给无人机的数据: " << payloadJson << std::endl;
        LOG(INFO) << "SCSN模块向无人机发送数据ID：" << temp.getDataSheetIdentificationNum();
        LOG(INFO) << "SCSN模块向无人机发送数据：" << payloadJson;
    }
}

void receiveCData(int HOST, ThreadSafeQueue<DataPack>& mobilityQueue, ThreadSafeQueue<DataPack>& uavSubQueue, ThreadSafeQueue<DataPack>& groupDirectiveExecutionQueue) {
    /**
    * @brief 接收来自c++的数据，根据DataPack的type判断下一步走哪里
    * @param HOST 开放的端口号
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
    serverAddr.sin_port = htons(HOST);  // 主机端口号
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

    std::cout << "等待客户端(无人机)连接..." << std::endl;

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
    while (true) {
        DataPack receivedPack;
        receivedPack.payload = static_cast<char*>(malloc(2000));
        char *receivedData = static_cast<char*>(malloc(2000));
        int bytesReceived = recv(clientSocket, receivedData, 2000, 0);
        memcpy(&receivedPack.header, receivedData, sizeof(receivedPack.header)); //复制头部

        memcpy(receivedPack.payload, receivedData + sizeof(receivedPack.header), receivedPack.getPackLength()); //复制payload
        
        if (bytesReceived == -1) {
            perror("Error receiving data");
            break;
        }

        if (bytesReceived == 0) {
            std::cout << "客户端断开连接" << std::endl;
            break;
        }
        
        // receivedPack.coutDataPackHeader();
        std::cout << "接收到来自无人机的数据:receivedPack.payload: " << receivedPack.payload <<  std::endl;
        std::cout << "接收到来自无人机的数据:receivedPack.getPackType(): " << receivedPack.getPackType() <<  std::endl;
        
        if(receivedPack.getPackType() == 9){
            mobilityQueue.push(receivedPack);  // 存进移动管理队列
        }
        else if (receivedPack.getPackType() == 2){
            groupDirectiveExecutionQueue.push(receivedPack);           
        }
        else if (receivedPack.getPackType() == 3){
            // 关机执行情况，给前端回调函数
            std::string response;
            std::string postData = receivedPack.payload;
            if (performPostRequest(OFFLINEURL, postData, response)) {
                std::cout << "尝试关机回调成功: " << response << std::endl;
            }else{
                std::cout << "尝试关机回调错误." << std::endl;
            }
        }
        else if(receivedPack.getPackType() == 1){
            // 飞行指令执行情况，给前端回调函数
            std::string response;
            std::string postData = receivedPack.payload;
            // "cmdNum:003,timestamp:1704446816276,code:2,message:b"
            // std::cout << "给金文林：" << postData << std::endl;
            if (performPostRequest(CALLBACKURL, postData, response)) {
                std::cout << "回调函数传输成功，返回为: " << response << std::endl;
            }else{
                std::cout << "回调函数传输错误." << std::endl;
            }
        }
        
        else if(find(begin(uavPackType), end(uavPackType), receivedPack.getPackType()) != end(uavPackType)){
            uavSubQueue.push(receivedPack);  // 存进无人机队列
        }
        else{
            std::cout << "接收到的数据类型错误" << std::endl;
        }
        // std::cout << "执行完了" << std::endl;
        free(receivedData);
    }

    // 关闭套接字
    close(clientSocket);
    close(serverSocket);
}

void listenMobilityQueue(ThreadSafeQueue<DataPack>& mobilityQueue){
    /**
    * @brief 监听移动性管理函数，将mobilityQueue中的数据（结构体）取出并进行操作
    * @param mobilityQueue 移动管理数据队列
    * @return None
    */
    while (true) {
        if (!mobilityQueue.empty()) {
            DataPack value = mobilityQueue.pop();
            #ifdef MOBILITY_COUT
                std::cout << "取出移动性管理模块数据并释放" << value.payload << std::endl;
                IPupdate_SaveToRedis_hash(value);
                free(value.payload); 
            #endif
            
            #ifdef MOBILITY_COUT
                //std::cout << "[Mobility]ID为1的无人机IP地址："  << Read_UAVinfo_IP(1) <<std::endl;
                Read_UAVinfo_IP_hash_all(2);
            #endif
        }
    }
}

void listenUavDataQueue(ThreadSafeQueue<DataPack>& uavSubQueue){
    /**
    * @brief 监听无人机数据函数，将uavSubQueue中的数据（结构体）取出并进行操作
    * @param uavSubQueue 无人机返回数据
    * @return Nome
    */
    while (true) {
        if (!uavSubQueue.empty()) {
            DataPack value = uavSubQueue.pop();
            // std::cout << "取出无人机返回数据并释放" << value.payload << std::endl;
            subinfo2Redis(REDISIP, REDISPORT, REDISPASSWORD, value.payload);
            free(value.payload); 
        }
    }
}

