#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cerrno>
#include <iostream>
#include <fstream> 
#include <thread>
#include <chrono>
#include "RedisConnect.h"
#include "json.hpp"

#include "scsnDataHead.h"

using namespace std;
using json = nlohmann::json;

struct UAV_mobility {
    int uav_id; //无人机的ID
    string uav_ip;  //无人机的IP
    int cluster_id; //簇号
};

/*--格式转换相关函数--*/
json StructToJson(const UAV_mobility& obj);
string JsonToString(const json& obj);
json StringToJson(const string& obj);
UAV_mobility JsonToStruct(json jsonObj);
/*--获取无人机信息--*/
//[待定]之后会有一份文档可以从中读取需要的ID和簇号，IP未知
UAV_mobility get_UAV_info();
/*--数据库操作部分--*/
shared_ptr<RedisConnect> ConnectToRedis(int redis_id);
void Write_UAVinfo(int key_int, string value);
void Write_UAVinfo_hash(UAV_mobility uav_info);
string Read_UAVinfo(int uavID);
string Read_UAVinfo_IP(int uavID);
string Read_UAVinfo_IP_hash(int clusterID, int uavID);
vector<string> Read_UAVinfo_IP_hash_all(int clusterID);
/*--与信息模块交互--*/
DataPack Pack_mobility(string uav_info);
/*--定时更新ip信息（无人机端）--*/
void IPupdate_SendToServer(ThreadSafeQueue<DataPack>& Queue);
void IPupdate_Time(ThreadSafeQueue<DataPack>& Queue);
/*--定时更新ip信息（服务器端）--*/
void IPupdate_SaveToRedis(DataPack uav_info);
void IPupdate_SaveToRedis_hash(DataPack uav_info);
/*--socket，测试用--*/
DataPack socket_server_datapack();
void socket_client_datapack(const DataPack &p);
