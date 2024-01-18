
#include "Commander.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using json = nlohmann::json;

extern ThreadSafeQueue<std::string> sharedCommandQueue; // fatsapi传过来的字符串指令(body)
extern ThreadSafeQueue<std::string> sendDataQueue; // 无人机编队处理后的
extern ThreadSafeQueue<DataPack> groupDirectiveExecutionQueue; // 编队指令执行情况

void Commander::getUserCommand() {
    // 获取用户输入的编队指令
    int inputCmd;
    while (true) {
        if (isCommandProcessed) {
            std::cout << "Enter command: ";
            isCommandProcessed = false;
        }
        if (std::cin >> inputCmd) {
            mtx.lock();
            cmd = inputCmd;
            mtx.unlock();
            if (cmd == 0) { // 退出命令
                break;
            }
        } else {
            std::cout << "Invalid command. Please try again." << std::endl;
            std::cin.clear(); // 清除错误状态
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 忽略错误输入直到下一个换行符
        }
    }
}


double Commander::distance_uav(const std::array<double, 3>& a, const std::array<double, 3>& b) {
    return std::sqrt(std::pow(a[0] - b[0], 2) + std::pow(a[1] - b[1], 2) + std::pow(a[2] - b[2], 2));
}

std::vector<std::vector<double>> Commander::calculateCostMatrix() {
    //外层向量大小是无人机架数（行），内层是目标点
    std::vector<std::vector<double>> costMatrix(uavs.size(), std::vector<double>(targets.size(), 0.0));
    for (size_t i = 0; i < uavs.size(); ++i) {
        for (size_t j = 0; j < targets.size(); ++j) {
            costMatrix[i][j] = distance_uav(uavs[i].position, targets[j]);
        }
    }
    return costMatrix;
}


void Commander::xiongAlgorithm() {
    //生成新编队位置
    auto costMatrix = calculateCostMatrix();
    Hungarian hungarian;
    vector<double> assignment;
    hungarian.solve(costMatrix, assignment);
    for (size_t i = 0; i < uavs.size(); ++i) {
        size_t targetIndex = static_cast<size_t>(assignment[i]);
        if (targetIndex < targets.size()) {
            std::array<double, 3> targetCoords = targets[targetIndex];
            // 更新无人机的目标位置
            uavs[i].target = targetCoords;
        }
    }
}

//     // 广播无人机的编号和坐标
//    void Commander::broadcastUAVData() {
//     int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sockfd < 0) {
//         std::cerr << "Error creating socket" << std::endl;
//         return;
//     }

//     struct sockaddr_in broadcastAddr;
//     memset(&broadcastAddr, 0, sizeof(broadcastAddr));
//     broadcastAddr.sin_family = AF_INET;
//     broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
//     broadcastAddr.sin_port = htons(1235);
    
//     int broadcastEnable = 1;
//     if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
//         std::cerr << "Error in setting Broadcast option" << std::endl;
//         close(sockfd);
//         return;
//     }

//     while (!infoAnalyToClusterManQueue.empty()) {
//     DataPack pack = infoAnalyToClusterManQueue.pop();
    
//     if (sendto(sockfd, pack.payload, pack.payloadSize, 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
//     std::cerr << "Error in sending Broadcast message" << std::endl;
//     //break;
//     }

//     delete[] pack.payload;
// }

//     close(sockfd);
// }

//键盘输入指令使用
// void Commander::process(const std::array<double, 3>& center, double angle, int n_uavs, double distance,const std::vector<Commander::UAV>& testUavs) {
//     std::thread inputThread(&Commander::getUserCommand, this);  // 处理键盘输入线程

//     while (true) {
//         int currentCommand = -1;

//         //读取当前命令
//         mtx.lock();
//         currentCommand = cmd;
//         mtx.unlock();

//         //如果当前命令与上一次命令不同，且为有效命令，则执行
//         if (currentCommand != lastCommand && currentCommand != -1) {
    void Commander::process(const std::array<double, 3>& center, double angle, int n_uavs, double distance,const std::vector<Commander::UAV>& testUavs, int currentCommand) {        
            
            if (currentCommand == 1) {  //切换直线编队 
                // 计算目标点
                std::vector<std::array<double, 3>> Targets = calculate_formation(center, angle, n_uavs, distance, currentCommand);
                setTestData(testUavs, Targets);
                xiongAlgorithm();         //编队切换
                //
                //addToQueue(uavs,infoAnalyToClusterManQueue);
                // for (const auto& uav : uavs) { 
                // std::cout << "UAV " << uav.id << " (" 
                // << uav.position[0] << ", " << uav.position[1] << ", " << uav.position[2]
                // << ")  ->  ("
                // << uav.target[0] << ", " << uav.target[1] << ", " << uav.target[2] << ")" << std::endl;
                // }
                //
                //按照addToQueue函数写的打印函数
                for (const auto& uav : uavs) {
                    //序列化数据，与addToQueue中相同
                    size_t bufferSize = sizeof(uav.id) + 2 * sizeof(std::array<double, 3>);
                    char* uavData = new char[bufferSize];
                    size_t offset = 0;
                    std::memcpy(uavData + offset, &uav.id, sizeof(uav.id));
                    offset += sizeof(uav.id);
                    std::memcpy(uavData + offset, &uav.position, sizeof(uav.position));
                    offset += sizeof(uav.position);
                    std::memcpy(uavData + offset, &uav.target, sizeof(uav.target));
                    double pos[3], target[3];
                    std::memcpy(pos, uavData + sizeof(uav.id), sizeof(pos));
                    std::memcpy(target, uavData + sizeof(uav.id) + sizeof(pos), sizeof(target));

                    //打印解析后的数据
                    // std::cout << "ID: " << uav.id<< std::endl;
                    // std::cout << "Position: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
                    // std::cout << "Target: " << target[0] << ", " << target[1] << ", " << target[2] << std::endl;
                    delete[] uavData; //释放内存
                }
                
                //broadcastUAVData();       //广播无人机数据
            }
            if (currentCommand == 2) {  //切换箭头编队 
                //计算目标点
                std::vector<std::array<double, 3>> Targets = calculate_formation(center, angle, n_uavs, distance, currentCommand);
                setTestData(testUavs, Targets);
                xiongAlgorithm();         //编队切换
                //
                //addToQueue(uavs,infoAnalyToClusterManQueue);
                for (const auto& uav : uavs) { 
                std::cout << "UAV " << uav.id << " (" 
                << uav.position[0] << ", " << uav.position[1] << ", " << uav.position[2]
                << ")  ->  ("
                << uav.target[0] << ", " << uav.target[1] << ", " << uav.target[2] << ")" << std::endl;
                }
                //
                //broadcastUAVData();       //广播无人机数据

            }
            if (currentCommand == 3) {  //切换斜线编队
                //计算目标点
                std::vector<std::array<double, 3>> Targets = calculate_formation(center, angle, n_uavs, distance, currentCommand);
                setTestData(testUavs, Targets);
                xiongAlgorithm();         //编队切换
                //
                //addToQueue(uavs,infoAnalyToClusterManQueue);
                for (const auto& uav : uavs) { 
                std::cout << "UAV " << uav.id << " (" 
                << uav.position[0] << ", " << uav.position[1] << ", " << uav.position[2]
                << ")  ->  ("
                << uav.target[0] << ", " << uav.target[1] << ", " << uav.target[2] << ")" << std::endl;
                }
                //
                //broadcastUAVData();       //广播无人机数据
            }        

        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 休眠
    }

// //封包，现在没什么用
// void Commander::addToQueue(const std::vector<Commander::UAV>& uavs, ThreadSafeQueue<DataPack>& queue) {
//     for (const auto& uav : uavs) {
//         size_t bufferSize = sizeof(uav.id) + 2 * sizeof(std::array<double, 3>);
        
//         char* uavData = new char[bufferSize]; 

//         size_t offset = 0;
//         //复制UAV数据
//         std::memcpy(uavData + offset, &uav.id, sizeof(uav.id));
//         offset += sizeof(uav.id);
//         std::memcpy(uavData + offset, &uav.position, sizeof(uav.position));
//         offset += sizeof(uav.position);
//         std::memcpy(uavData + offset, &uav.target, sizeof(uav.target));

//         DataPack pack;
//         pack.setPackType(16); //包类型为16
//         pack.payload = uavData;
//         pack.payloadSize = bufferSize;

//         queue.push(pack); //将DataPack加到队列中
//     }
// }

void Commander::broadcastPauseCommand() {
    //std::cout << "stop" << std::endl;
}

void Commander::broadcastResumeCommand() {
    //std::cout << "fly" << std::endl;
}
//测试匈牙利算法给定输入输出，输出各无人机位置
void Commander::testAlgorithm() {
    std::array<double, 3> center = {0.0, 0.0, 0.0};
    double angle= 45;
    int n_uavs= 6;
    double distance= 1.0;
    std::cout<< "1  2D line  2  2D arrow  3  3D slash" << std::endl;
    std::cin >> cmd;
    std::vector<std::array<double, 3>> testTargets = calculate_formation(center, angle, n_uavs, distance, cmd);

     //测试
     //打印目标点集合
    std::cout << "........................." << std::endl;
    std::cout << "Target Points:" << std::endl;
    
    for (const auto& target : testTargets) {
        std::cout << "(" << target[0] << ", " << target[1] << ", " << target[2] << ")" << std::endl;
    }
    std::cout << "........................." << std::endl;
    std::cout << "switching process:" << std::endl;
    std::vector<Commander::UAV> testUavs= {
        {1, {1.8, 2.8, 3.8}, {0, 0, 0}},
        {2, {1.0, 2, 2}, {0, 0, 0}},
        {3, {2, 2, 2}, {0, 0, 0}},
        {4, {2, 2, 2}, {0, 0, 0}},
        {5, {2, 2, 2}, {0, 0, 0}},
        {6, {2, 2, 2}, {0, 0, 0}}
    };
    // std::vector<std::array<double, 3>> testTargets = {
    //     {3.5, 4.6, 3.2},
    //     {4.1, 4.7, 6.5},
    //     {5.5, 2.4, 8.3},
    //     {3, 2.4, 8.3},
    //     {4, 2.4, 8.3},
    //     {5.5, 5, 2},
    // };
    setTestData(testUavs, testTargets);
    // xyl算法
    xiongAlgorithm();
    for (const auto& uav : uavs) { 
       std::cout << "UAV " << uav.id << " (" 
              << uav.position[0] << ", " << uav.position[1] << ", " << uav.position[2]
              << ")  ->  ("
              << uav.target[0] << ", " << uav.target[1] << ", " << uav.target[2] << ")" << std::endl;
               //  std::cout<<uav.position[0]<< ", " << uav.position[1] << ", " << uav.position[2];
    }
}

//计算两点间角度
double Commander::calculateAngle(const json& sParams, const json& dParams) {
    double deltaX = dParams["x"].get<double>() - sParams["x"].get<double>();
    double deltaY = dParams["y"].get<double>() - sParams["y"].get<double>();
    return std::atan2(deltaY, deltaX) * (180.0 / M_PI); // 角度转换为度
}

void Commander::processJsonData(Commander& commander, std::string& jsonData, ThreadSafeQueue<std::string>& sendDataQueue,ThreadSafeQueue<std::string>& queueTemp) {
    std::replace(jsonData.begin(), jsonData.end(), '\'', '\"');
    json root = json::parse(jsonData);

    int lineFormat = root["lineFormat"].get<int>();
    std::string leaderId = root["leader"].get<std::string>();
    json data = root["data"];

    if (lineFormat == 0) {
        // 处理每个无人机的数据
        for (const auto& uavData : data) {
            std::string uavString = uavData.dump();
            json uavJson = json::parse(uavString);
            uavJson["packType"] = 18;
            uavString = uavJson.dump();
            std::cout << "unchanged_data:" << uavString << std::endl;
            sendDataQueue.push(uavString);
        }
    } else {
        // 计算编队信息
        std::array<double, 3> scenter;//source
        std::array<double, 3> dcenter;//destination
        int n_uavs = data.size();
        std::vector<Commander::UAV> testUavs;
        double angle = 0.0;

        for (const auto& uavData : data) {
            if (uavData["uid"].get<std::string>() == leaderId) {
                scenter[0] = uavData["sParams"]["x"].get<double>();
                scenter[1] = uavData["sParams"]["y"].get<double>();
                scenter[2] = uavData["sParams"]["z"].get<double>();
                dcenter[0] = uavData["dParams"]["x"].get<double>();
                dcenter[1] = uavData["dParams"]["y"].get<double>();
                dcenter[2] = uavData["dParams"]["z"].get<double>();
                angle = calculateAngle(uavData["sParams"], uavData["dParams"]);
            }

            Commander::UAV uav;
            uav.id = std::stoi(uavData["uid"].get<std::string>());
            uav.position = {uavData["sParams"]["x"].get<double>(), uavData["sParams"]["y"].get<double>(), uavData["sParams"]["z"].get<double>()};
            testUavs.push_back(uav);
        }
        double distance = 1.0; 
        //计算新的编队目标点   queue3计算包含了161
        //auto formationPoints = commander.calculate_formation(dcenter, angle, n_uavs, distance, lineFormat);
       
        commander.process(scenter, angle, n_uavs, distance, testUavs, lineFormat);

        // 更新 dParams 并放入 queue2
        for (size_t i = 0; i <uavs.size(); ++i) {
            data[i]["dParams"]["x"] = uavs[i].target[0];
            data[i]["dParams"]["y"] = uavs[i].target[1];
            data[i]["dParams"]["z"] = uavs[i].target[2];
            data[i]["packType"] = 16;
            Commander::UAV uav1;
            uav1.id = i +1;
            uav1.position= {uavs[i].target[0],uavs[i].target[1],uavs[i].target[2]};//第一次切换结果作为第二次的初始位置
            uavs_fp.push_back(uav1);
            std::string update_data=data[i].dump();
            std::cout<<"update_data:"<<update_data<<endl;
            sendDataQueue.push(update_data);
        }
        //queue3 更改初始无人机集合uavs_fp
        commander.process(dcenter, angle, n_uavs, distance, uavs_fp, lineFormat);
        // 更新 dParams 并放入临时队列
        for (size_t i = 0; i <uavs.size(); ++i) {
            data[i]["dParams"]["x"] = uavs[i].target[0];
            data[i]["dParams"]["y"] = uavs[i].target[1];
            data[i]["dParams"]["z"] = uavs[i].target[2];
            data[i]["packType"] = 17;
            std::string update_data2=data[i].dump();
            //std::cout<<"update_data2:"<<update_data2<<endl;
            queueTemp.push(update_data2);
        }
    }
}

//处理返回的单个无人机队列，提取code值赋给key
void Commander::processQueue(ThreadSafeQueue<DataPack>& queue, int& key) {
    while (!queue.empty()) {
        DataPack dataPack = queue.pop();
        // 使用 strstr 查找 "code:"
        const char* codePosition = std::strstr(dataPack.payload, "code:");
        char code;
        if (codePosition != nullptr) {
            // 提取 "code" 后的字符部分
            
            if (std::sscanf(codePosition + 5, "%c", &code) == 1) {
                std::cout << "Code value: " << code << std::endl;
            } else {
                std::cerr << "Failed to parse code value." << std::endl;
            }
        } else {
            std::cerr << "Code not found in the input string." << std::endl;
        }

        //cout<<"code = "<<code;
        if (code == '0') {
            key = 0;
        } else if (code == '1') {
            key = 1;
        } 
        // else if (code == "2") {
        //     key = 2;            
        // }
        //cout<<"key="<<key;
        // 清理payload指向内存
        delete[] dataPack.payload;

    }
}

int formation(ThreadSafeQueue<std::string>& sharedCommandQueue, ThreadSafeQueue<std::string>& sendDataQueue) {
    Commander commander;
    ThreadSafeQueue<std::string> queueTemp;

    while (true) {
        // 判断是否进行第二部飞行
        int key = -1;
        if (!sharedCommandQueue.empty()) {
            std::string jsonData = sharedCommandQueue.pop();
            commander.processJsonData(commander, jsonData, sendDataQueue, queueTemp);
        }
        commander.processQueue(groupDirectiveExecutionQueue, key);
        if(key == 1){          
            while(!queueTemp.empty()){
                std::string jsonData = queueTemp.pop();
                std::cout<<"update_data_2"<<jsonData<< std::endl;
                sendDataQueue.push(jsonData);               
            }
            
        }else if(key == 0){
                std::cout << "code = 0,只输出第一次切换的数据" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
