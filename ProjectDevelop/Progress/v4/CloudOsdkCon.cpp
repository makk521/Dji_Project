// 只在妙算上运行，
// 运行顺序为先运行，为保证post成功，post函数先停10秒再运行，即需要10秒之内运行CloudThread.py
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include "RedisConnect.h"
#include "nlohmann/json.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

using json = nlohmann::json;
using namespace std;

const int RECEIVEPORT = 5001;   //socket开放端口
struct Vector3f {
    float x;
    float y;
    float z;
};

struct Person {
    string name;
    int age;
};

// 重载版本1：将 Vector3f 转换为 JSON
json structToJson(const Vector3f& obj) {
    json jsonObj;
    jsonObj["x"] = obj.x;
    jsonObj["y"] = obj.y;
    jsonObj["z"] = obj.z;
    return jsonObj;
}

// 重载版本2：将 Person 结构体转换为 JSON
json structToJson(const Person& obj) {
    json jsonObj;
    jsonObj["name"] = obj.name;
    jsonObj["age"] = obj.age;
    return jsonObj;
}

void timerSetRedis() {
    """
    连接数据库后,每隔0.1秒向数据库写入一次数据(Json->String写入数据库)
    """
    string val;
    int UAVIP = 001;
    int DATA = 1;
    RedisConnect::Setup("172.21.2.7", 6379, "Ustc1958@2023");
    shared_ptr<RedisConnect> redis = RedisConnect::Instance();
    redis->select(11);

    Vector3f myVector = {1.0f, 2.0f, 3.0f};
    Person person = {"John", 18};
    json vectorJson = structToJson(myVector);
    json personJson = structToJson(person);

    json myJson;
    myJson["myVector"] = vectorJson;
    myJson["personJson"] = personJson;
    cout << myJson << endl;
    string jsonString = myJson.dump();
    while (true) {
        // 在这里执行你的任务
        if(UAVIP == 005){
            UAVIP = 001;
        }
        redis->set("ma_test:" + to_string(UAVIP) + "$", jsonString);
        UAVIP++;
        DATA++;
        cout << "插入成功一次" << endl;
        this_thread::sleep_for(chrono::seconds(5));
    }
}

"""
队列模板,重写pop与push指令,支持定义的队列存取int、string等基本数据类型
"""
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
    """
    先延时10秒,之后与服务端socket连接,每隔1秒发送一次数据
    """
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
    """
    socket服务端,被连接后若接收到发送来的数据,将其push进shareQueue中
    """
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

void consumerFun(ThreadSafeQueue<std::string>& sharedQueue) {  // 指令Json，
    """
    一直等待shareQueue中的数据,将其取出(无人机指令及参数)
    """
    for (int i = 0; i < 10; ++i) {
        std::string value = sharedQueue.pop();  // Use pop method
        std::cout << "Consumed: " << value << std::endl;
    }
}

int main() {
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

    // 创建Socket
    int serverSocketReceiver = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketReceiver == -1) {
        perror("Error creating socket");
        return 1;
    }

    // 绑定IP地址和端口
    sockaddr_in serverAddrReceiver;
    serverAddrReceiver.sin_family = AF_INET;
    serverAddrReceiver.sin_port = htons(RECEIVEPORT); // 使用端口12345
    serverAddrReceiver.sin_addr.s_addr = INADDR_ANY; // 监听所有网卡上的连接

    thread posterThread(postData, clientSocketPoster, serverAddrPoster);
    thread receiverThread(receiveData, serverSocketReceiver, serverAddrReceiver, std::ref(sharedCommandQueue));
    // thread redisThread(timerSetRedis);
    thread consumerThread(consumerFun, std::ref(sharedCommandQueue));

    // 等待接收线程完成
    receiverThread.join();
    posterThread.join();
    // redisThread.join();

    // 关闭 Socket
    close(clientSocketPoster);
    close(serverSocketReceiver);

    return 0;
}