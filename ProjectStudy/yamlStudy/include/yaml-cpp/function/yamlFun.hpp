#include <iostream>
#include <fstream> // 用于写yaml
#include "yaml-cpp/yaml.h"
#include <chrono>
#include "json.hpp"

using json = nlohmann::json;

extern YAMLPATH = "uav_data.yaml";

int64_t getTimestamp(); // 获取13位时间戳