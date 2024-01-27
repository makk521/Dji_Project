#include <iostream>
#include <fstream> // 用于写yaml
#include "yaml-cpp/yaml.h"
#include <chrono>
#include "json.hpp"

using json = nlohmann::json;

int64_t getTimestamp() {
    /**
    * @brief 获取13位时间戳
    * @param 
    * @return 
    */
    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return ms.count();
}

int main() {
    // 读取 YAML 文件
    YAML::Node config = YAML::LoadFile("uav_data.yaml");

    // 检查是否成功加载文件
    if (config.IsNull()) {
        std::cerr << "无法加载 YAML 文件！" << std::endl;
        return 1;
    }

    // 从 YAML 文件中获取数据
    std::string uavMac = config["uavMac"].as<std::string>();
    std::string uavIp = config["uavIp"].as<std::string>();

    // 打印读取到的数据
    std::cout << "uavMac: " << uavMac << std::endl;
    std::cout << "uavIp: " << uavIp << std::endl;
    int64_t timeStamp = getTimestamp();
    std::cout << "timeStamp: " << timeStamp << std::endl;


    json j
    {
        {"uavMac", uavMac},
        {"uavIp", uavIp},
        {"timeStamp", timeStamp}
    };
    std::cout << j.dump() << std::endl;
    return 0;
}
