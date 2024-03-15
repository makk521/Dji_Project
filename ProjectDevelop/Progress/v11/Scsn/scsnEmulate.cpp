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
#include "Commander.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;

extern const int RECEIVEFROMPYTHONPORT;   // 对python的开放端口
extern const int RECEIVEFROMCPORT; // 对c++的开放端口
extern const int uavPackType[]; //无人机返回数据包的type
extern const string CALLBACKURL;
extern const string OFFLINEURL;
extern const std::string REDISIP;
extern const int REDISPORT;
extern const std::string REDISPASSWORD;

ThreadSafeQueue<DataPack> mobilityQueue; // 存放来自右侧的移动性管理模块数据
ThreadSafeQueue<DataPack> uavSubQueue; // 存放来自无人机的返回数据
extern ThreadSafeQueue<std::string> sharedCommandQueue; // fatsapi传过来的字符串指令(body)
extern ThreadSafeQueue<std::string> sendDataQueue; // 无人机编队处理后的
extern ThreadSafeQueue<DataPack> groupDirectiveExecutionQueue; // 编队指令执行情况
const std::string UAVIP = "192.168.10.83"; // 无人机端的IP地址
const int UAVPORT = 8001; // 无人机端开放的端口


void initLogger(){
    /**
    * @brief 配置名为"myLogger"的日志记录器，使其将所有级别的日志消息写入到文件myeasylog.log(运行位置)中,默认名称
    * @param None
    * @return None
    */
    el::Configurations conf;
    conf.setToDefault();
    el::Loggers::reconfigureLogger("myLogger", conf);
}


int main() {
    /**
    * @brief 主函数，负责创建、启动与管理线程，包括socket的收发两个线程,等待队列中指令发送给无人机线程
    * @param None
    * @return 0
    */
    initLogger();
    LOG(INFO) << "This is an info log in main";
    // 创建 Socket Poster
    int clientSocketPoster = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketPoster == -1) {
        perror("Error creating socket");
        return 1;
    }

    // 设置PYTHON(回传)的 IP 地址和端口号 Poster 已不需要
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
    uavAddrPoster.sin_port = htons(UAVPORT); // 替换为服务器的端口号
    uavAddrPoster.sin_addr.s_addr = inet_addr(UAVIP.c_str()); // 替换为服务器的 IP 地址

    // 创建Socket
    int serverSocketReceiver = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketReceiver == -1) {
        perror("Error creating socket");
        return 1;
    }

    // 绑定IP地址和端口
    sockaddr_in serverAddrReceiver;
    serverAddrReceiver.sin_family = AF_INET;
    serverAddrReceiver.sin_port = htons(RECEIVEFROMPYTHONPORT); // 使用端口5001
    serverAddrReceiver.sin_addr.s_addr = INADDR_ANY; // 监听所有网卡上的连接

    // 初始化Redis
    RedisInit(REDISIP, REDISPORT, REDISPASSWORD);
    // 回传给python
    // std::thread posterThread(postToPythonData, clientSocketPoster, serverAddrPoster);
    // 接收python数据并将string存入sharedCommandQueue
    std::thread receiverPythonThread(receivePythonData, serverSocketReceiver, serverAddrReceiver, std::ref(sharedCommandQueue));
    // 马际清监听sharedCommandQueue并处理编队信息，将单个无人机的指令(字符串)放入sendDataQueue
    std::thread multiProgressThread(formation, std::ref(sharedCommandQueue), std::ref(sendDataQueue));
    // 监听sendDataQueue并将其中数据打包发送给无人机
    std::thread consumerThread(consumerFun, std::ref(sendDataQueue), uavSocketPoster, uavAddrPoster);
    // 接收来自c++的返回数据包括无人机的订阅信息、执行情况等，还有移动性管理模块的返回数据
    std::thread receiveCDataThread(receiveCData, RECEIVEFROMCPORT, std::ref(mobilityQueue), std::ref(uavSubQueue), std::ref(groupDirectiveExecutionQueue)); // 开放端口5002
    // 监听移动性管理模块的返回数据
    std::thread listenMobilityQueueThread(listenMobilityQueue, std::ref(mobilityQueue));
    // 监听无人机的订阅信息，并存入数据库
    std::thread listenUavDataQueueThread(listenUavDataQueue, std::ref(uavSubQueue));

    // 等待接收线程完成
    receiverPythonThread.join();
    // posterThread.join();
    consumerThread.join();
    multiProgressThread.join();
    receiveCDataThread.join();
    listenMobilityQueueThread.join();
    listenUavDataQueueThread.join();
    // 关闭 Socket
    close(clientSocketPoster);
    close(serverSocketReceiver);

    return 0;
}
