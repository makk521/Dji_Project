#include "yamlFun.hpp"

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

json readYaml(const std::string &yamlPath){
    /**
    * @brief 读取yaml文件获取无人机信息
    * @param yamlPath 文件地址
    * @return 返回json格式的无人机信息
    */
    YAML::Node config = YAML::LoadFile(yamlPath);

    // 检查是否成功加载文件
    if (config.IsNull()) {
        std::cerr << "无法加载 YAML 文件！" << std::endl;
        return 1;
    }

    // 从 YAML 文件中获取数据
    std::string uavMac = config["uavMac"].as<std::string>();
    std::string uavIp = config["uavIp"].as<std::string>();
    json yamlData;
    yamlData["uavMac"] = uavMac;
    yamlData["uavTimestamp"] = getTimestamp();
    yamlData["uavId"] = uavIp;

}