/*
可以运行在linux中直接，每1秒刷新一次数据库，添加的是多个结构体结合的Json格式，即最终格式
*/
#include <iostream>
#include <thread>
#include <chrono>
#include "RedisConnect.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

struct Vector3f {
    float x;
    float y;
    float z;
};

struct Person {
    std::string name;
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

// 要执行的函数
void myFunction() {
    string val;
    int UAVIP = 001;
    int DATA = 1;
    RedisConnect::Setup("172.21.2.1", 6379, "Ustc1958@2023");
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
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    // 创建一个单独的线程，用于执行myFunction
    thread myThread(myFunction);

    // 主线程可以执行其他任务

    // 等待myThread完成
    myThread.join();

    return 0;
}
