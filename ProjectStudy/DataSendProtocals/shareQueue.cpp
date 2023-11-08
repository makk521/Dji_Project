"""
定义共享队列,其中生产者将json以字符串的形式put进队列,消费者取出字符串后以转为json,利用索引解析数据
妙算平台：
/home/dji/Desktop/ma/redisConnect/StructToJson
"""
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "myStructs.hpp"
#include "JsonConversion.cpp"
#include <iostream>

using namespace std;

json jsonRedisType;
int num = 1;

std::queue<std::string> myQueue; // 队列用于存储字符串
std::mutex mtx; // 用于保护队列的互斥量
std::condition_variable cv; // 用于线程同步的条件变量

void producer() {
    RC rcObj = {0, 0, 0, 0, 0, -0};
    json jsonRc = structToJson(rcObj);
    VelocityInfo velocityInfoObj = {1, 63}; // 示例值，health为1，reserve为63
    json jsonVelocityInfo = structToJson(velocityInfoObj);
    jsonRedisType["RC"] = jsonRc;
    jsonRedisType["VelocityInfo"] = jsonVelocityInfo;
    string jsonString = jsonRedisType.dump();

    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(mtx); // 锁定互斥量
        myQueue.push(jsonString); // 将字符串放入队列
        cout << "Produce: " << jsonString << endl;
        cv.notify_one(); // 通知等待的消费者线程
    }
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx); // 锁定互斥量
        cv.wait(lock, [] { return !myQueue.empty(); }); // 等待队列不为空
        std::string jsonString = myQueue.front(); // 获取队列头部字符串
        json jsonData = json::parse(jsonString); // 字符串转Json

        myQueue.pop(); // 移除头部字符串
        lock.unlock(); // 解锁互斥量

        std::cout << "Consumed: " << jsonData["RC"] << std::endl;
    }
}

int main() {
    std::thread producerThread(producer);
    std::thread consumerThread(consumer);

    producerThread.join();
    consumerThread.join();

    return 0;
}
