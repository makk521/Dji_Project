/**
* @file dataPack.cpp
* @author Makaka
* @date 2023-12-19
* @brief 定义使用的全局变量，防止重定义
*/
#include "dataPack.h"

ThreadSafeQueue<DataPack> scsnQueue;
ThreadSafeQueue<DataPack> scsnToInfoAnalyQueue; // SCSN-->信息解析模块
ThreadSafeQueue<DataPack> infoAnalyToClusterManQueue; // 信息解析模块-->集群管理模块
ThreadSafeQueue<DataPack> infoAnalyToCommProcQueue;  // 信息解析模块-->指令处理模块
ThreadSafeQueue<DataPack> infoAnalyToDataProcQueue; // 信息解析模块-->数据处理模块
ThreadSafeQueue<DataPack> infoAnalyToRouteManQueue; // 信息解析模块-->路由管理模块
// 模块返回数据无法使用地址
ThreadSafeQueue<DataPack> otherModuToInfoAnalyQueue; // 集群管理模块、数据处理模块、路由管理模块-->信息解析模块
ThreadSafeQueue<DataPack> commProcToInfoAnalyQueue; // 指令处理模块-->信息解析模块
ThreadSafeQueue<DataPack> subDataToCommProcQueue; // 订阅信息-->指令处理模块(CommProcToInfoAnalyQueue)

ThreadSafePriorityQueue<DataPack> commProcPriorityQueue;