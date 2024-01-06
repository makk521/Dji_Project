
/*! @file flight-control/main.cpp
 *  @version 4.0.0
 *  @date Jun 05 2017
 *
 *  @brief
 *  main for Flight Control API usage in a Linux environment.
 *  Provides a number of helpful additions to core API calls,
 *  especially for position control, attitude control, takeoff,
 *  landing.
 *
 *  @Copyright (c) 2016-2017 DJI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

/*TODO:flight_control_sample will by replace by flight_sample in the future*/
#include "flight_control_sample.hpp"
#include "dji_linux_helpers.hpp"
#include "flight_sample.hpp"
#include <chrono>
#include <future>
#include <thread>
#include <vector>
#include <sstream>
#include "nlohmann/json.hpp"
#include <string>
#include "dataPack.h"
#include <map>

using json = nlohmann::json;

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;

extern uint8_t flightStatus;
extern bool isArmed; 
extern bool isDisarmed;
extern bool isFlying;
json getCurrentDateTime(json& jsonObj, const std::string& key) {
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
    auto currentTimeA = std::chrono::system_clock::to_time_t(now);
    jsonObj[key]["timestamp"] = currentTime;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&currentTimeA), "%Y-%m-%d %H:%M:%S");
    jsonObj[key]["formatted_time"] = ss.str();
    return jsonObj;
}
std::string getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
    return std::to_string(currentTime);
}


json& structToJson(json& jsonObj, uint8_t obj, const std::string& key)
{
    jsonObj[key] = static_cast<int>(obj);
    return jsonObj;
}

json& structToJson(json& jsonObj, uint16_t obj, const std::string& key)
{
    jsonObj[key] = static_cast<int>(obj);
    return jsonObj;
}

json& structToJson(json& jsonObj, int16_t obj, const std::string& key)
{
    jsonObj[key] = obj;
    return jsonObj;
}

json& structToJson(json& jsonObj, uint32_t obj, const std::string& key)
{
    jsonObj[key] = static_cast<unsigned long>(obj);
    return jsonObj;
}

json& structToJson(json& jsonObj, float32_t obj, const std::string& key)
{
    jsonObj[key] = static_cast<double>(obj);
    return jsonObj;
}

json structToJson(json& jsonObj, const Quaternion& obj, const std::string& key)
{
    jsonObj[key]["q0"] = obj.q0;
    jsonObj[key]["q1"] = obj.q1;
    jsonObj[key]["q2"] = obj.q2;
    jsonObj[key]["q3"] = obj.q3;
    return jsonObj;
}

json structToJson(json& jsonObj, const Vector3f& obj, const std::string& key)
{
    jsonObj[key]["x"] = obj.x;
    jsonObj[key]["y"] = obj.y;
    jsonObj[key]["z"] = obj.z;
    return jsonObj;
}

json structToJson(json& jsonObj, const GPSFused& obj, const std::string& key)
{
    jsonObj[key]["longitude"] = obj.longitude;
    jsonObj[key]["latitude"] = obj.latitude;
    jsonObj[key]["altitude"] = obj.altitude;
    jsonObj[key]["visibleSatelliteNumber"] = obj.visibleSatelliteNumber;
    return jsonObj;
}

json structToJson(json& jsonObj, const Vector3d& obj, const std::string& key)
{
    jsonObj[key]["x"] = obj.x;
    jsonObj[key]["y"] = obj.y;
    jsonObj[key]["z"] = obj.z;
    return jsonObj;
}

json structToJson(json& jsonObj, const VelocityInfo& obj, const std::string& key)
{
    jsonObj[key]["health"] = obj.health;
    jsonObj[key]["reserve"] = obj.reserve;
    return jsonObj;
}

json structToJson(json& jsonObj, const Velocity& obj, const std::string& key)
{
    jsonObj[key]["data"] = structToJson(jsonObj[key]["data"], obj.data, "Vector3f");
    jsonObj[key]["info"] = structToJson(jsonObj[key]["info"], obj.info, "VelocityInfo");
    return jsonObj;
}

json structToJson(json& jsonObj, const GPSDetail& obj, const std::string& key)
{
    jsonObj[key]["hdop"] = obj.hdop;
    jsonObj[key]["pdop"] = obj.pdop;
    jsonObj[key]["fix"] = obj.fix;
    jsonObj[key]["gnssStatus"] = obj.gnssStatus;
    jsonObj[key]["hacc"] = obj.hacc;
    jsonObj[key]["sacc"] = obj.sacc;
    jsonObj[key]["usedGPS"] = obj.usedGPS;
    jsonObj[key]["usedGLN"] = obj.usedGLN;
    jsonObj[key]["NSV"] = obj.NSV;
    jsonObj[key]["GPScounter"] = obj.GPScounter;
    return jsonObj;
}

json structToJson(json& jsonObj, const PositionData& obj, const std::string& key)
{
    jsonObj[key]["longitude"] = obj.longitude;
    jsonObj[key]["latitude"] = obj.latitude;
    jsonObj[key]["HFSL"] = obj.HFSL;
    return jsonObj;
}

json structToJson(json& jsonObj, const Mag& obj, const std::string& key)
{
    jsonObj[key]["x"] = obj.x;
    jsonObj[key]["y"] = obj.y;
    jsonObj[key]["z"] = obj.z;
    return jsonObj;
}
json structToJson(json& jsonObj, const RC& obj, const std::string& key)
{
    jsonObj[key]["roll"] = obj.roll;
    jsonObj[key]["pitch"] = obj.pitch;
    jsonObj[key]["yaw"] = obj.yaw;
    jsonObj[key]["throttle"] = obj.throttle;
    jsonObj[key]["mode"] = obj.mode;
    jsonObj[key]["gear"] = obj.gear;
    return jsonObj;
}

json structToJson(json& jsonObj, const GimbalStatus& obj, const std::string& key)
{
    jsonObj[key]["mountStatus"] = obj.mountStatus;
    jsonObj[key]["isBusy"] = obj.isBusy;
    jsonObj[key]["pitchLimited"] = obj.pitchLimited;
    jsonObj[key]["rollLimited"] = obj.rollLimited;
    jsonObj[key]["yawLimited"] = obj.yawLimited;
    jsonObj[key]["calibrating"] = obj.calibrating;
    jsonObj[key]["prevCalibrationgResult"] = obj.prevCalibrationgResult;
    jsonObj[key]["installedDirection"] = obj.installedDirection;
    jsonObj[key]["disabled_mvo"] = obj.disabled_mvo;
    jsonObj[key]["gear_show_unable"] = obj.gear_show_unable;
    jsonObj[key]["gyroFalut"] = obj.gyroFalut;
    jsonObj[key]["escPitchStatus"] = obj.escPitchStatus;
    jsonObj[key]["escRollStatus"] = obj.escRollStatus;
    jsonObj[key]["escYawStatus"] = obj.escYawStatus;
    jsonObj[key]["droneDataRecv"] = obj.droneDataRecv;
    jsonObj[key]["initUnfinished"] = obj.initUnfinished;
    jsonObj[key]["FWUpdating"] = obj.FWUpdating;
    return jsonObj;
}

json structToJson(json& jsonObj, const Battery& obj, const std::string& key)
{
    jsonObj[key]["capacity"] = obj.capacity;
    jsonObj[key]["voltage"] = obj.voltage;
    jsonObj[key]["current"] = obj.current;
    jsonObj[key]["percentage"] = obj.percentage;
    return jsonObj;
}

json structToJson(json& jsonObj, const SDKInfo& obj, const std::string& key)
{
    jsonObj[key]["controlMode"] = obj.controlMode;
    jsonObj[key]["deviceStatus"] = obj.deviceStatus;
    jsonObj[key]["flightStatus"] = obj.flightStatus;
    jsonObj[key]["vrcStatus"] = obj.vrcStatus;
    jsonObj[key]["reserved"] = obj.reserved;
    return jsonObj;
}

json structToJson(json& jsonObj, const SyncTimestamp& obj, const std::string& key)
{
    jsonObj[key]["time2p5ms"] = obj.time2p5ms;
    jsonObj[key]["time1ns"] = obj.time1ns;
    jsonObj[key]["resetTime2p5ms"] = obj.resetTime2p5ms;
    jsonObj[key]["index"] = obj.index;
    jsonObj[key]["flag"] = obj.flag;
    return jsonObj;
}

json structToJson(json& jsonObj, const HardSyncData& obj, const std::string& key)
{
    jsonObj[key]["ts"] = structToJson(jsonObj[key]["ts"], obj.ts, "SyncTimestamp");
    jsonObj[key]["q"] = structToJson(jsonObj[key]["q"], obj.q, "Quaternion");
    jsonObj[key]["a"] = structToJson(jsonObj[key]["a"], obj.a, "Vector3f");
    jsonObj[key]["w"] = structToJson(jsonObj[key]["w"], obj.w, "Vector3f");
    return jsonObj;
}

json structToJson(json& jsonObj, const RCWithFlagData& obj, const std::string& key)
{
    jsonObj[key]["pitch"] = obj.pitch;
    jsonObj[key]["roll"] = obj.roll;
    jsonObj[key]["yaw"] = obj.yaw;
    jsonObj[key]["throttle"] = obj.throttle;
    jsonObj[key]["flag"]["logicConnected"] = obj.flag.logicConnected;
    jsonObj[key]["flag"]["skyConnected"] = obj.flag.skyConnected;
    jsonObj[key]["flag"]["groundConnected"] = obj.flag.groundConnected;
    jsonObj[key]["flag"]["appConnected"] = obj.flag.appConnected;
    jsonObj[key]["flag"]["reserved"] = obj.flag.reserved;
    return jsonObj;
}

json structToJson(json& jsonObj, const ESCStatusIndividual& obj, const std::string& key)
{
    jsonObj[key]["current"] = obj.current;
    jsonObj[key]["speed"] = obj.speed;
    jsonObj[key]["voltage"] = obj.voltage;
    jsonObj[key]["temperature"] = obj.temperature;
    jsonObj[key]["stall"] = obj.stall;
    jsonObj[key]["empty"] = obj.empty;
    jsonObj[key]["unbalanced"] = obj.unbalanced;
    jsonObj[key]["escDisconnected"] = obj.escDisconnected;
    jsonObj[key]["temperatureHigh"] = obj.temperatureHigh;
    jsonObj[key]["reserved"] = obj.reserved;
    return jsonObj;
}

json structToJson(json& jsonObj, const EscData& obj, const std::string& key)
{
    for (int i = 0; i < MAX_ESC_NUM; i++) {
        jsonObj[key]["esc"][i] = structToJson(jsonObj[key]["esc"][i], obj.esc[i], "ESCStatusIndividual");
    }
    return jsonObj;
}

json structToJson(json& jsonObj, const RTKConnectStatus& obj, const std::string& key)
{
    jsonObj[key]["rtkConnected"] = obj.rtkConnected;
    jsonObj[key]["reserve"] = obj.reserve;
    return jsonObj;
}

json structToJson(json& jsonObj, const FlightAnomaly& obj, const std::string& key)
{
    jsonObj[key]["impactInAir"] = obj.impactInAir;
    jsonObj[key]["randomFly"] = obj.randomFly;
    jsonObj[key]["heightCtrlFail"] = obj.heightCtrlFail;
    jsonObj[key]["rollPitchCtrlFail"] = obj.rollPitchCtrlFail;
    jsonObj[key]["yawCtrlFail"] = obj.yawCtrlFail;
    jsonObj[key]["aircraftIsFalling"] = obj.aircraftIsFalling;
    jsonObj[key]["strongWindLevel1"] = obj.strongWindLevel1;
    jsonObj[key]["strongWindLevel2"] = obj.strongWindLevel2;
    jsonObj[key]["compassInstallationError"] = obj.compassInstallationError;
    jsonObj[key]["imuInstallationError"] = obj.imuInstallationError;
    jsonObj[key]["escTemperatureHigh"] = obj.escTemperatureHigh;
    jsonObj[key]["atLeastOneEscDisconnected"] = obj.atLeastOneEscDisconnected;
    jsonObj[key]["gpsYawError"] = obj.gpsYawError;
    jsonObj[key]["reserved"] = obj.reserved;
    return jsonObj;
}

json structToJson(json& jsonObj, const LocalPositionVO& obj, const std::string& key)
{
    jsonObj[key]["x"] = obj.x;
    jsonObj[key]["y"] = obj.y;
    jsonObj[key]["z"] = obj.z;
    jsonObj[key]["xHealth"] = obj.xHealth;
    jsonObj[key]["yHealth"] = obj.yHealth;
    jsonObj[key]["zHealth"] = obj.zHealth;
    jsonObj[key]["reserved"] = obj.reserved;
    return jsonObj;
}

json structToJson(json& jsonObj, const DJI::OSDK::Telemetry::RelativePosition& obj, const std::string& key)
{
    jsonObj[key]["down"] = obj.down;
    jsonObj[key]["front"] = obj.front;
    jsonObj[key]["right"] = obj.right;
    jsonObj[key]["back"] = obj.back;
    jsonObj[key]["left"] = obj.left;
    jsonObj[key]["up"] = obj.up;
    jsonObj[key]["downHealth"] = obj.downHealth;
    jsonObj[key]["frontHealth"] = obj.frontHealth;
    jsonObj[key]["rightHealth"] = obj.rightHealth;
    jsonObj[key]["backHealth"] = obj.backHealth;
    jsonObj[key]["leftHealth"] = obj.leftHealth;
    jsonObj[key]["upHealth"] = obj.upHealth;
    jsonObj[key]["reserved"] = obj.reserved;
    return jsonObj;
}
json structToJson(json& jsonObj, const HomeLocationSetStatus& obj, const std::string& key)
{
    jsonObj[key]["status"] = obj.status;
    return jsonObj;
}

json structToJson(json& jsonObj, const HomeLocationData& obj, const std::string& key)
{
    jsonObj[key]["latitude"] = obj.latitude;
    jsonObj[key]["longitude"] = obj.longitude;
    return jsonObj;
}

json structToJson(json& jsonObj){
  jsonObj["isArmed"] = isArmed;
  jsonObj["isDisarmed"] = isDisarmed;
  jsonObj["isFlying"] = isFlying;
  return jsonObj;
}


void
ObtainJoystickCtrlAuthorityCB(ErrorCode::ErrorCodeType errorCode,
                              UserData                 userData)
{
  if (errorCode == ErrorCode::FlightControllerErr::SetControlParam::
                     ObtainJoystickCtrlAuthoritySuccess)
  {
    DSTATUS("ObtainJoystickCtrlAuthoritySuccess");
  }
}

void
ReleaseJoystickCtrlAuthorityCB(ErrorCode::ErrorCodeType errorCode,
                               UserData                 userData)
{
  if (errorCode == ErrorCode::FlightControllerErr::SetControlParam::
                     ReleaseJoystickCtrlAuthoritySuccess)
  {
    DSTATUS("ReleaseJoystickCtrlAuthoritySuccess");
  }
}





DataPack Pack_send(std::string sub_info){
    DataPack dp;
    std::size_t jsonStringLength = sub_info.size();
    dp.setPackLength(224+(jsonStringLength + 1)*8);
    // dp.setPackType(0);
    dp.payload = new char[jsonStringLength + 1]; // 需要额外的1个字符来存放字符串结束符 '\0'
    // std::strcpy(dp.payload, jsonString.c_str());// 将 JSON 字符串复制到 payload 
    std::copy(sub_info.begin(), sub_info.end(), dp.payload);// 使用 std::copy 将 JSON 字符串复制到 payload 中
    dp.payload[jsonStringLength] = '\0';// 添加字符串的结束符 '\0'
    return dp;
}
void Pack_mobilityA(string uav_info,DataPack *dp){
    int len=uav_info.length();
    dp->setPackLength(224+(len+1)*8);//设置包长度，单位为bit
    //dp->payload = (char*)uav_info.c_str();
    dp->payload = new char[len+1];
    strcpy(dp->payload,uav_info.c_str());
    cout<<dp->payload<<endl;
}

//获取无人机的状态信息
std::string generateStatus() {
    std::string statusString = "isArmed: " + std::to_string(isArmed) + ", isDisarmed: " + std::to_string(isDisarmed) +
                               ", isFlying: " + std::to_string(isFlying);
    return statusString;
}
std::string generateStatusAsJSON() {
    json statusJson;
    statusJson["isArmed"] = isArmed;
    statusJson["isDisarmed"] = isDisarmed;
    statusJson["isFlying"] = isFlying;
    return statusJson.dump();
}
json getStatus(json& jsonObj) {
    jsonObj["flightStatus"] = flightStatus;
    return jsonObj;
}
int order(int argc, char** argv, ThreadSafePriorityQueue<DataPack>& commProcPriorityQueue)
{
  //std::cout << value << std::endl;
  // Initialize variables
  int functionTimeout = 1;
  // std::cout<<argc<<endl;
  //const char* charargv[] = { "djiosdk-flightcontrol-sample", "UserConfig.txt", "UserConfig.txt" };

  // 做一个简单的转换，将 const char* 转换为 char*
  //char** argv = const_cast<char**>(charargv);
  //int argc = sizeof(argv) / sizeof(argv[0]);
  // Setup OSDK.
  LinuxSetup linuxEnvironment(argc, argv);
  Vehicle*   vehicle = linuxEnvironment.getVehicle();
  if (vehicle == NULL)
  {
    std::cout << "Vehicle not initialized, exiting.\n";
    return -1;
  }

  // Obtain Control Authority
  // ErrorCode::ErrorCodeType ret =
  // vehicle->flightController->obtainJoystickCtrlAuthoritySync(functionTimeout);
  vehicle->flightController->obtainJoystickCtrlAuthorityAsync(
    ObtainJoystickCtrlAuthorityCB, nullptr, functionTimeout, 2);
  FlightSample* flightSample = new FlightSample(vehicle);

  int timeout  = 10;
  // int pkgIndex = 0;
  int freq     = 5;

  if (vehicle->isLegacyM600()) {
      std::cout << "This is an M600 drone." << std::endl;
  } else if (vehicle->isM100()) {
      std::cout << "This is an M100 drone." << std::endl;
  } else if (vehicle->isM210V2()) {
      std::cout << "This is an M210 V2 drone." << std::endl;
  } else if (vehicle->isM300()) {
      std::cout << "This is an M300 drone." << std::endl;
  } else {
      // 如果无法确定型号
      std::cout << "Unknown drone model." << std::endl;
  }

  // json time;
  // getCurrentDateTime(time,"TIME_NOW");
  // std::cout << time.dump() << '\n';

  TopicName topicList1[] = {
    TOPIC_GPS_DATE,//GPS日期信息5
    TOPIC_GPS_TIME,//GPS时间信息5
    TOPIC_GPS_POSITION,//GPS位置数据5
    TOPIC_GPS_VELOCITY,//GPS速度数据5
    TOPIC_GPS_DETAILS,//GPS详细信息，如精度、卫星数等5
    TOPIC_RTK_POSITION,//RTK（Real-Time Kinematic）定位的位置数据5
    TOPIC_RTK_VELOCITY,//RTK速度数据5
    TOPIC_RTK_YAW,//RTK的偏航角数据5
    TOPIC_RTK_POSITION_INFO,//RTK定位信息5
    TOPIC_RTK_YAW_INFO,//RTK偏航角信息5
    TOPIC_BATTERY_INFO,//电池信息。50
    // TOPIC_CONTROL_DEVICE//控制设备信息   没显示hz
  };

  TopicName topicList2[] = {
    TOPIC_QUATERNION,//四元数数据，通常用于表示飞行器的姿态（姿势）200
    TOPIC_ACCELERATION_GROUND,//飞行器在地面坐标系中的加速度数据200
    TOPIC_ACCELERATION_BODY,//飞行器在机体坐标系中的加速度数据200
    TOPIC_ACCELERATION_RAW,//原始加速度数据，可能是传感器直接输出的数据400
    TOPIC_VELOCITY,//飞行器的速度数据200
    TOPIC_ANGULAR_RATE_FUSIONED,//融合后的角速度数据200
    TOPIC_ANGULAR_RATE_RAW,//原始角速度数据400
    TOPIC_ALTITUDE_FUSIONED,//融合后的高度数据200
    TOPIC_ALTITUDE_BAROMETER,//气压计测得的高度数据200
    TOPIC_HEIGHT_FUSION,//高度数据的融合100    
    TOPIC_GPS_FUSED//GPS数据的融合，包括经纬度等信息50
  };


  TopicName topicList3[] = {
    TOPIC_COMPASS,//罗盘数据100
    TOPIC_RC,//遥控器输入数据100
    TOPIC_GIMBAL_ANGLES,//云台角度数据50
    TOPIC_GIMBAL_STATUS,//云台状态数据50
    TOPIC_STATUS_FLIGHT,//飞行状态数据50
    TOPIC_STATUS_DISPLAYMODE,//显示模式状态数据50
    TOPIC_STATUS_LANDINGGEAR,//起落架状态数据50
    TOPIC_STATUS_MOTOR_START_ERROR,//发动机启动错误状态数据50
    TOPIC_HARD_SYNC,//硬件同步数据400
    TOPIC_GPS_SIGNAL_LEVEL,//GPS信号强度50
    TOPIC_GPS_CONTROL_LEVEL//GPS控制级别50
  };

  TopicName topicList4[] = {
    //TOPIC_RC_FULL_RAW_DATA, //deprecated 已经被弃用了50   
    TOPIC_RC_WITH_FLAG_DATA,//带有标志数据的遥控器输入数据（TOPIC_RC_FULL_RAW_DATA已弃用）50
    TOPIC_ESC_DATA,//电调器（ESC）数据50
    TOPIC_RTK_CONNECT_STATUS,//RTK连接状态50
    TOPIC_GIMBAL_CONTROL_MODE,//云台控制模式50
    TOPIC_FLIGHT_ANOMALY,//飞行异常数据50
    TOPIC_POSITION_VO,//视觉避障位置数据50
    TOPIC_AVOID_DATA//避障数据100
  };

  TopicName topicList5[] = {
    TOPIC_ALTITUDE_OF_HOMEPOINT,//相对于Home点的高度数据     没显示hz，并且提示是数据在每次起飞时更新
    TOPIC_HOME_POINT_SET_STATUS,//Home点设置状态    没显示hz
    TOPIC_CONTROL_DEVICE,//控制设备信息   没显示hz
    TOPIC_HOME_POINT_INFO//Home点信息     没显示hz
  };



  int freq1 = 5;
  int pkgIndex1 = 1;
  int topicSize1 = sizeof(topicList1) / sizeof(topicList1[0]);
  flightSample->setUpSubscription(pkgIndex1, freq1, topicList1, topicSize1, timeout);

  int freq2 = 10;
  int pkgIndex2 = 2;
  int topicSize2 = sizeof(topicList2) / sizeof(topicList2[0]);
  flightSample->setUpSubscription(pkgIndex2, freq2, topicList2, topicSize2, timeout);

  int freq3 = 10;
  int pkgIndex3 = 3;
  int topicSize3 = sizeof(topicList3) / sizeof(topicList3[0]);
  flightSample->setUpSubscription(pkgIndex3, freq3, topicList3, topicSize3, timeout);        

  int freq4 = 10;
  int pkgIndex4 = 4;
  int topicSize4 = sizeof(topicList4) / sizeof(topicList4[0]);
  flightSample->setUpSubscription(pkgIndex4, freq4, topicList4, topicSize4, timeout);

  int freq5 = 1;
  int pkgIndex5 = 5;
  int topicSize5 = sizeof(topicList5) / sizeof(topicList5[0]);
  flightSample->setUpSubscription(pkgIndex5, freq5, topicList5, topicSize5, timeout);
  // for (int i = 0; i < 2; ++i){
  //   json mergedJson;
  //   getCurrentDateTime(mergedJson,"TIME_NOW");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_POSITION>(), "TOPIC_GPS_POSITION");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>(), "TOPIC_GPS_DETAILS");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>(), "TOPIC_BATTERY_INFO");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>(), "TOPIC_CONTROL_DEVICE");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_VELOCITY>(), "TOPIC_VELOCITY");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>(), "TOPIC_ALTITUDE_FUSIONED");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>(), "TOPIC_ALTITUDE_BAROMETER");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>(), "TOPIC_HEIGHT_FUSION");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_FUSED>(), "TOPIC_GPS_FUSED");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_MOTOR_START_ERROR>(), "TOPIC_STATUS_MOTOR_START_ERROR");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_FLIGHT_ANOMALY>(), "TOPIC_FLIGHT_ANOMALY");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_AVOID_DATA>(), "TOPIC_AVOID_DATA");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>(), "TOPIC_ALTITUDE_OF_HOMEPOINT");
  //   structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HOME_POINT_INFO>(), "TOPIC_HOME_POINT_INFO");
  //   getStatus(mergedJson);
  //   std::cout<<mergedJson.dump(4)<<std::endl;
  //   std::cout<<"一轮结束"<<std::endl;
  // }

  // 创建一个线程来持续执行代码
  std::thread continuousThread([=]() {
      while (true) {
          // 持续执行代码段
          auto last_iteration_time = std::chrono::steady_clock::now();
          json mergedJson;
          getCurrentDateTime(mergedJson,"TIME_NOW");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_POSITION>(), "TOPIC_GPS_POSITION");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>(), "TOPIC_GPS_DETAILS");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>(), "TOPIC_BATTERY_INFO");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>(), "TOPIC_CONTROL_DEVICE");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_VELOCITY>(), "TOPIC_VELOCITY");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>(), "TOPIC_ALTITUDE_FUSIONED");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>(), "TOPIC_ALTITUDE_BAROMETER");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>(), "TOPIC_HEIGHT_FUSION");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_FUSED>(), "TOPIC_GPS_FUSED");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_MOTOR_START_ERROR>(), "TOPIC_STATUS_MOTOR_START_ERROR");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_FLIGHT_ANOMALY>(), "TOPIC_FLIGHT_ANOMALY");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_AVOID_DATA>(), "TOPIC_AVOID_DATA");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>(), "TOPIC_ALTITUDE_OF_HOMEPOINT");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HOME_POINT_INFO>(), "TOPIC_HOME_POINT_INFO");
          getStatus(mergedJson);
          std::cout << mergedJson.dump(4) << '\n';
          std::string jsonString = mergedJson.dump();
          // std::cout << jsonString << '\n';
          // std::string jsonString = mergedJson.dump();
          // DataPack Info = Pack_send(jsonString);//最终的包Info
          // subDataToCommProcQueue.push(Info);
          // delete[] Info.payload;//释放 payload 的内存
          DataPack *dp;
          dp = new DataPack();
          dp->setPackType(0);
          Pack_mobilityA(jsonString,dp);
          cout<<dp->payload<<endl;
          subDataToCommProcQueue.push(*dp);
          // 计算已经过去的时间
          auto current_time = std::chrono::steady_clock::now();
          auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_iteration_time).count();
          // 计算需要等待的时间，确保在每次迭代中等待1秒
          auto wait_time = 5000 - elapsed_time;
          if (wait_time > 0) {
              std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
          }
          else{
            std::cout <<"time false\n";
          }
          last_iteration_time = std::chrono::steady_clock::now(); // 更新上一次迭代的时间
          std::cout<<"一轮结束\n";    
      }
  });

  std::string uid = "1";

  while (1)
  {
    // std::cout << "cmdType:\n";
    // 0上锁、1解锁、2起飞、3降落、4指点飞行、5返航、6设置返航高度、7紧急制动两秒
    DataPack value = commProcPriorityQueue.pop();
    std::string payloadString = value.payload;
    json payloadJson = json::parse(payloadString);    
    // int cmdType;
    // std::cin >> cmdType;
    //switch (std::stoi(std::string(payloadJson["commandNum"])))
    // int commandNum = std::stoi(payloadJson["commandNum"].get<std::string>());
    // int commandNum = payloadJson["commandNum"].get<int>();
    std::string uid = payloadJson["uid"].get<std::string>();
    std::string action = payloadJson["action"].get<std::string>();
    std::string cmdNum = payloadJson["commandNum"].get<std::string>();
    std::string timeStamp = payloadJson["timeStamp"].get<std::string>();
    std::map<std::string, float> params = payloadJson["params"];
    // json paramsJson = json::parse(params); 
      if(action == "disarm")
      {
        // 上锁
        std::cout << "dis arm\n";
        vehicle->control->disArmMotors();
        flightStatus = 1;
        std::string statusM = "1";// 用于存储状态信息的字符串
        std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
        DataPack *dp;
        dp = new DataPack();
        dp->setPackType(1);
        Pack_mobilityA(statusMsg,dp);
        cout<<dp->payload<<endl;
        subDataToCommProcQueue.push(*dp);        
        isArmed = false;
        isFlying = false;
      }
      else if(action == "arm") 
      {
        // 解锁
        std::cout << "arm\n";
        vehicle->control->armMotors();
        flightStatus = 2;
        std::string statusM = "1";// 用于存储状态信息的字符串
        std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
        DataPack *dp;
        dp = new DataPack();
        dp->setPackType(1);
        Pack_mobilityA(statusMsg,dp);
        cout<<dp->payload<<endl;
        subDataToCommProcQueue.push(*dp);        
        isArmed = false;
        isFlying = false;
      }
      else if(action == "takeoff")
      {
        std::string statusMsg; // 声明 statusMsg 作用域内可见
        std::string statusM; // 声明 statusM 作用域内可见
        float nums[4];
        // 创建一个 promise 和一个 future 来获取函数的执行状态
        std::promise<bool> promiseResult;
        std::future<bool> futureResult = promiseResult.get_future();

        // 在线程中执行函数，并将执行结果通过 promise 传递给 future
        std::thread threadA([flightSample, payloadJson](std::promise<bool>& promise) {
            bool result = flightSample->monitoredTakeoff();
            promise.set_value(result); // 将函数执行结果传递给 promise
        }, std::ref(promiseResult));

        // 等待函数执行完成，并获取执行结果
        std::future_status status = futureResult.wait_for(std::chrono::seconds(0));
        while (status != std::future_status::ready) {
            // std::cout << "指令正在执行..." << std::endl;
            std::string statusM = "2";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
            // DataPack Ies = Pack_send(statusMsg);//指令执行情况的包Ies
            // subDataToCommProcQueue.push(Ies);
            // delete[] Ies.payload;//释放 payload 的内存
            DataPack *dp;
            dp = new DataPack();
            dp->setPackType(1);
            Pack_mobilityA(statusMsg,dp);
            cout<<dp->payload<<endl;
            subDataToCommProcQueue.push(*dp);
            std::this_thread::sleep_for(std::chrono::seconds(10)); // 每隔十秒检查一次状态
            status = futureResult.wait_for(std::chrono::seconds(0));
        }    
        bool executionResult = futureResult.get();// 获取函数的执行结果
        // 根据执行结果做相应的处理
        if (executionResult) {
            // std::cout << "函数执行成功！" << std::endl;
            std::string statusM = "1";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
        } else {
            // std::cout << "函数执行失败或超时！" << std::endl;
            std::string statusM = "0";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
        }
        DataPack *dp;
        dp = new DataPack();
        dp->setPackType(1);
        Pack_mobilityA(statusMsg,dp);
        cout<<dp->payload<<endl;
        subDataToCommProcQueue.push(*dp);
        threadA.join();
      }
      else if(action == "land")
      {
        std::string statusMsg; // 声明 statusMsg 作用域内可见
        std::string statusM; // 声明 statusM 作用域内可见
        float nums[4];
        // 创建一个 promise 和一个 future 来获取函数的执行状态
        std::promise<bool> promiseResult;
        std::future<bool> futureResult = promiseResult.get_future();

        // 在线程中执行函数，并将执行结果通过 promise 传递给 future
        std::thread threadA([flightSample, payloadJson](std::promise<bool>& promise) {
            bool result = flightSample->monitoredLanding();
            promise.set_value(result); // 将函数执行结果传递给 promise
        }, std::ref(promiseResult));

        // 等待函数执行完成，并获取执行结果
        std::future_status status = futureResult.wait_for(std::chrono::seconds(0));
        while (status != std::future_status::ready) {
            // std::cout << "指令正在执行..." << std::endl;
            std::string statusM = "2";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
            std::cout << statusMsg << std::endl;
            // DataPack Ies = Pack_send(statusMsg);//指令执行情况的包Ies
            // subDataToCommProcQueue.push(Ies);
            // delete[] Ies.payload;//释放 payload 的内存
            DataPack *dp;
            dp = new DataPack();
            dp->setPackType(1);
            Pack_mobilityA(statusMsg,dp);
            cout<<dp->payload<<endl;
            subDataToCommProcQueue.push(*dp);
            std::this_thread::sleep_for(std::chrono::seconds(10)); // 每隔十秒检查一次状态
            status = futureResult.wait_for(std::chrono::seconds(0));
        }    
        bool executionResult = futureResult.get();// 获取函数的执行结果
        // 根据执行结果做相应的处理
        if (executionResult) {
            // std::cout << "函数执行成功！" << std::endl;
            std::string statusM = "1";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
        } else {
            // std::cout << "函数执行失败或超时！" << std::endl;
            std::string statusM = "0";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
        }
        DataPack *dp;
        dp = new DataPack();
        dp->setPackType(1);
        Pack_mobilityA(statusMsg,dp);
        cout<<dp->payload<<endl;
        subDataToCommProcQueue.push(*dp);
        threadA.join();
      }
      else if(action == "move")
      {
        std::string statusMsg; // 声明 statusMsg 在 switch 作用域内可见
        std::string statusM; // 声明 statusM 在 switch 作用域内可见
        std::cout << "move:please input x,y,z,yaw offset\n";
        float nums[4];
        // 创建一个 promise 和一个 future 来获取函数的执行状态
        std::promise<bool> promiseResult;
        std::future<bool> futureResult = promiseResult.get_future();

        // 在线程中执行函数，并将执行结果通过 promise 传递给 future
        std::thread threadA([flightSample, payloadJson,params](std::promise<bool>& promise) {
            bool result = flightSample->moveByPositionOffset((FlightSample::Vector3f){ params.at("x") , params.at("y") , params.at("z")  },params.at("yaw"),0.8,1);
            promise.set_value(result); // 将函数执行结果传递给 promise
        }, std::ref(promiseResult));

        // 等待函数执行完成，并获取执行结果
        std::future_status status = futureResult.wait_for(std::chrono::seconds(0));
        while (status != std::future_status::ready) {
            // std::cout << "指令正在执行..." << std::endl;
            std::string statusM = "2";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
            // DataPack Ies = Pack_send(statusMsg);//指令执行情况的包Ies
            // subDataToCommProcQueue.push(Ies);
            // delete[] Ies.payload;//释放 payload 的内存
            DataPack *dp;
            dp = new DataPack();
            dp->setPackType(1);
            Pack_mobilityA(statusMsg,dp);
            cout<<dp->payload<<endl;
            subDataToCommProcQueue.push(*dp);
            std::this_thread::sleep_for(std::chrono::seconds(10)); // 每隔十秒检查一次状态
            status = futureResult.wait_for(std::chrono::seconds(0));
        }       
        bool executionResult = futureResult.get();// 获取函数的执行结果
        // 根据执行结果做相应的处理
        if (executionResult) {
            // std::cout << "函数执行成功！" << std::endl;
            std::string statusM = "1";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
        } else {
            // std::cout << "函数执行失败或超时！" << std::endl;
            std::string statusM = "0";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
        }
        DataPack *dp;
        dp = new DataPack();
        dp->setPackType(1);
        Pack_mobilityA(statusMsg,dp);
        cout<<dp->payload<<endl;
        subDataToCommProcQueue.push(*dp);
        threadA.join();
      }
      else if(action == "gohome")
      {
        std::string statusMsg; // 声明 statusMsg 作用域内可见
        std::string statusM; // 声明 statusM 作用域内可见
        float nums[4];
        // 创建一个 promise 和一个 future 来获取函数的执行状态
        std::promise<bool> promiseResult;
        std::future<bool> futureResult = promiseResult.get_future();

        // 在线程中执行函数，并将执行结果通过 promise 传递给 future
        std::thread threadA([flightSample, payloadJson](std::promise<bool>& promise) {
            bool result = flightSample->goHomeAndConfirmLanding();
            promise.set_value(result); // 将函数执行结果传递给 promise
        }, std::ref(promiseResult));

        // 等待函数执行完成，并获取执行结果
        std::future_status status = futureResult.wait_for(std::chrono::seconds(0));
        while (status != std::future_status::ready) {
            // std::cout << "指令正在执行..." << std::endl;
            std::string statusM = "2";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
            // DataPack Ies = Pack_send(statusMsg);//指令执行情况的包Ies
            // subDataToCommProcQueue.push(Ies);
            // delete[] Ies.payload;//释放 payload 的内存
            DataPack *dp;
            dp = new DataPack();
            dp->setPackType(1);
            Pack_mobilityA(statusMsg,dp);
            cout<<dp->payload<<endl;
            subDataToCommProcQueue.push(*dp);
            std::this_thread::sleep_for(std::chrono::seconds(10)); // 每隔十秒检查一次状态
            status = futureResult.wait_for(std::chrono::seconds(0));
        }    
        bool executionResult = futureResult.get();// 获取函数的执行结果
        // 根据执行结果做相应的处理
        if (executionResult) {
            // std::cout << "函数执行成功！" << std::endl;
            std::string statusM = "1";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
        } else {
            // std::cout << "函数执行失败或超时！" << std::endl;
            std::string statusM = "0";// 用于存储状态信息的字符串
            std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
            std::cout << statusMsg << std::endl;
        }
        DataPack *dp;
        dp = new DataPack();
        dp->setPackType(1);
        Pack_mobilityA(statusMsg,dp);
        cout<<dp->payload<<endl;
        subDataToCommProcQueue.push(*dp);
        threadA.join();
      }
      else if(action == "setGohomeHeight")
      {
        float nums[4];
        ErrorCode::ErrorCodeType result = flightSample->setGoHomeAltitude(params["height"]);
        if (result == 0) {
          std::string statusM = "1";// 用于存储状态信息的字符串
          std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
          DataPack *dp;
          dp = new DataPack();
          dp->setPackType(1);
          Pack_mobilityA(statusMsg,dp);
          cout<<dp->payload<<endl;
          subDataToCommProcQueue.push(*dp);
        } else {
          std::string statusM = "0";// 用于存储状态信息的字符串
          std::string statusMsg = "{cmdNum:" + cmdNum + "," + "uid:" + uid + "," + "action:" + action + "," + "timeStamp:" + timeStamp +  "," + "message:" + statusM + "}";
          DataPack *dp;
          dp = new DataPack();
          dp->setPackType(1);
          Pack_mobilityA(statusMsg,dp);
          cout<<dp->payload<<endl;
          subDataToCommProcQueue.push(*dp);
        }
      }
      // else if(action == "setGohomeHeight")
      // {
      //   // 设置返航高度
      //   // std::cout << "set go home altitude:please input altitude\n";
      //   // float height;
      //   std::cout << "move:please input x,y,z,yaw offset\n";
      //   std::cout << payloadJson["params"];
      //   float nums[4];
      //   // std::cin >> height;
      //   flightSample->setGoHomeAltitude(payloadJson["params"][0]);
      // }
      else if(action == "7")
      {
        flightSample->emergencyBrake();
        sleep(2);

      }
    //   else if(action == "8")
    //   {
    //     std::string statusMsg; // 声明 statusMsg 在 switch 作用域内可见
    //     std::string statusM; // 声明 statusM 在 switch 作用域内可见
    //     std::cout << "move:please input x,y,z,yaw offset\n";
    //     float nums[4];
    //     // 创建一个 promise 和一个 future 来获取函数的执行状态
    //     std::promise<bool> promiseResult;
    //     std::future<bool> futureResult = promiseResult.get_future();

    //     // 在线程中执行函数，并将执行结果通过 promise 传递给 future
    //     std::thread threadA([flightSample, payloadJson](std::promise<bool>& promise) {
    //         bool result = flightSample->moveByPositionOffset((FlightSample::Vector3f){ payloadJson["params"][0], payloadJson["params"][1], payloadJson["params"][2] },payloadJson["params"][3],0.8,1);
    //         promise.set_value(result); // 将函数执行结果传递给 promise
    //     }, std::ref(promiseResult));

    //     // 等待函数执行完成，并获取执行结果
    //     std::future_status status = futureResult.wait_for(std::chrono::seconds(0));
    //     while (status != std::future_status::ready) {
    //         // std::cout << "指令正在执行..." << std::endl;
    //         statusM = "2";// 用于存储状态信息的字符串
    //         statusMsg = statusM + ":" + action + ":" + getCurrentDateTime();
    //         std::cout << statusMsg << std::endl;
    //         // DataPack Ies = Pack_send(statusMsg);//指令执行情况的包Ies
    //         // subDataToCommProcQueue.push(Ies);
    //         // delete[] Ies.payload;//释放 payload 的内存
    //         DataPack *dp;
    //         dp = new DataPack();
    //         dp->setPackType(1);
    //         Pack_mobilityA(statusMsg,dp);
    //         cout<<dp->payload<<endl;
    //         subDataToCommProcQueue.push(*dp);
    //         std::this_thread::sleep_for(std::chrono::seconds(10)); // 每隔十秒检查一次状态
    //         status = futureResult.wait_for(std::chrono::seconds(0));
    //     }       
    //     bool executionResult = futureResult.get();// 获取函数的执行结果
    //     // 根据执行结果做相应的处理
    //     if (executionResult) {
    //         // std::cout << "函数执行成功！" << std::endl;
    //         statusM = "1";// 用于存储状态信息的字符串
    //         statusMsg = statusM + ":" + action + ":" + getCurrentDateTime();
    //         std::cout << statusMsg << std::endl;
    //     } else {
    //         // std::cout << "函数执行失败或超时！" << std::endl;
    //         statusM = "0";// 用于存储状态信息的字符串
    //         statusMsg = statusM + ":" + action + ":" + getCurrentDateTime();
    //         std::cout << statusMsg << std::endl;
    //     }
    //     DataPack *dp;
    //     dp = new DataPack();
    //     dp->setPackType(1);
    //     Pack_mobilityA(statusMsg,dp);
    //     cout<<dp->payload<<endl;
    //     subDataToCommProcQueue.push(*dp);
    //     threadA.join();
    //   }
    //   else if(action == "9")
    //   {
    //     std::string status = generateStatus();//用于存储无人机状态的字符串
    //     std::cout << status << std::endl;
    //     // DataPack Sta = Pack_send(status);//指令执行情况的包Sta
    //     // subDataToCommProcQueue.push(Sta);
    //     // delete[] Sta.payload;//释放 payload 的内存
    //     DataPack *dp;
    //     dp = new DataPack();
    //     Pack_mobilityA(status,dp);
    //     cout<<dp->payload<<endl;
    //     subDataToCommProcQueue.push(*dp);

    //   }
    //   else if(action == "10")
    //   {
    //     int freq1 = 5;
    //     int pkgIndex1 = 1;
    //     int topicSize1 = sizeof(topicList1) / sizeof(topicList1[0]);
    //     flightSample->setUpSubscription(pkgIndex1, freq1, topicList1, topicSize1, timeout);

    //     int freq2 = 10;
    //     int pkgIndex2 = 2;
    //     int topicSize2 = sizeof(topicList2) / sizeof(topicList2[0]);
    //     flightSample->setUpSubscription(pkgIndex2, freq2, topicList2, topicSize2, timeout);

    //     int freq3 = 10;
    //     int pkgIndex3 = 3;
    //     int topicSize3 = sizeof(topicList3) / sizeof(topicList3[0]);
    //     flightSample->setUpSubscription(pkgIndex3, freq3, topicList3, topicSize3, timeout);        

    //     int freq4 = 10;
    //     int pkgIndex4 = 4;
    //     int topicSize4 = sizeof(topicList4) / sizeof(topicList4[0]);
    //     flightSample->setUpSubscription(pkgIndex4, freq4, topicList4, topicSize4, timeout);

    //     int freq5 = 1;
    //     int pkgIndex5 = 5;
    //     int topicSize5 = sizeof(topicList5) / sizeof(topicList5[0]);
    //     flightSample->setUpSubscription(pkgIndex5, freq5, topicList5, topicSize5, timeout);

    //   }
    //   else if(action == "11")
    //   {
    //     auto last_iteration_time = std::chrono::steady_clock::now();
    //     for (int i = 0; i < 10; ++i) {
    //         // 启动异步任务执行操作
    //         json mergedJson;
    //         std::future<void> future = std::async(std::launch::async, [&](){
    //         getCurrentDateTime(mergedJson,"TIME_NOW");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_POSITION>(), "TOPIC_GPS_POSITION");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>(), "TOPIC_GPS_DETAILS");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>(), "TOPIC_BATTERY_INFO");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>(), "TOPIC_CONTROL_DEVICE");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_VELOCITY>(), "TOPIC_VELOCITY");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>(), "TOPIC_ALTITUDE_FUSIONED");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>(), "TOPIC_ALTITUDE_BAROMETER");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>(), "TOPIC_HEIGHT_FUSION");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_FUSED>(), "TOPIC_GPS_FUSED");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_MOTOR_START_ERROR>(), "TOPIC_STATUS_MOTOR_START_ERROR");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_FLIGHT_ANOMALY>(), "TOPIC_FLIGHT_ANOMALY");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_AVOID_DATA>(), "TOPIC_AVOID_DATA");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>(), "TOPIC_ALTITUDE_OF_HOMEPOINT");
    //         structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HOME_POINT_INFO>(), "TOPIC_HOME_POINT_INFO");
    //         getStatus(mergedJson);
    //         // std::string status = generateStatusAsJSON();//用于存储无人机状态的字符串
    //         // std::string jsonString = mergedJson.dump() + status;
    //         std::cout << mergedJson.dump(4) << '\n';
    //         std::string jsonString = mergedJson.dump();
    //         // std::cout << jsonString << '\n';
    //         // std::string jsonString = mergedJson.dump();
    //         // DataPack Info = Pack_send(jsonString);//最终的包Info
    //         // subDataToCommProcQueue.push(Info);
    //         // delete[] Info.payload;//释放 payload 的内存
    //         DataPack *dp;
    //         dp = new DataPack();
    //         dp->setPackType(0);
    //         Pack_mobilityA(jsonString,dp);
    //         cout<<dp->payload<<endl;
    //         subDataToCommProcQueue.push(*dp);
    //         });
    //         // 计算已经过去的时间
    //         auto current_time = std::chrono::steady_clock::now();
    //         auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_iteration_time).count();
    //         // 计算需要等待的时间，确保在每次迭代中等待0.1秒
    //         auto wait_time = 1000 - elapsed_time;
    //         if (wait_time > 0) {
    //             std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
    //         }
    //         else{
    //           std::cout <<"time false\n";
    //         }
    //         last_iteration_time = std::chrono::steady_clock::now(); // 更新上一次迭代的时间
    //         std::cout<<"一轮结束\n";
    //     }



    //   }
    //   else if(action == "12")
    //   {
    //     std::thread threadA([flightSample]() {flightSample->moveByPositionOffset((FlightSample::Vector3f){ 40, 40, 40 },1,0.8,1);});
    //     std::thread threadB([=]() {
    //         auto last_iteration_time = std::chrono::steady_clock::now();
    //         for (int i = 0; i < 100; ++i) {
    //             // 启动异步任务执行操作
    //             json mergedJson;
    //             std::future<void> future = std::async(std::launch::async, [&](){
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DATE>(), "TOPIC_GPS_DATE");
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_TIME>(), "TOPIC_GPS_TIME");
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_POSITION>(), "TOPIC_GPS_POSITION");
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_VELOCITY>(), "TOPIC_GPS_VELOCITY");
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>(), "TOPIC_GPS_DETAILS");
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_POSITION>(), "TOPIC_RTK_POSITION");
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>(), "TOPIC_RTK_VELOCITY");
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_YAW>(), "TOPIC_RTK_YAW");
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_POSITION_INFO>(), "TOPIC_RTK_POSITION_INFO");
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_YAW_INFO>(), "TOPIC_RTK_YAW_INFO");
    //             structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>(), "TOPIC_BATTERY_INFO");
    //             std::cout << mergedJson.dump() << '\n';
    //             });

    //             // 计算已经过去的时间
    //             auto current_time = std::chrono::steady_clock::now();
    //             auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_iteration_time).count();
    //             // 计算需要等待的时间，确保在每次迭代中等待0.1秒
    //             auto wait_time = 100 - elapsed_time;
    //             if (wait_time > 0) {
    //                 std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
    //             }
    //             else{
    //               std::cout <<"time false\n";
    //             }
    //             last_iteration_time = std::chrono::steady_clock::now(); // 更新上一次迭代的时间
    //             std::cout<<"一轮结束\n";
                
    //         }
    //     });
    //     threadA.join();
    //     threadB.join();
    //   }
    //   else if(action == "13")
    //   {
    //     std::cout << "move:please input x,y,z,yaw offset\n";
    //     std::cout << payloadJson["params"];
    //     float nums[4];
    //     // flightSample->monitoredTakeoff();
    //     DSTATUS("Take off over!\n");
    //     flightSample->moveByPositionOffset((FlightSample::Vector3f){ payloadJson["params"][0], payloadJson["params"][1], payloadJson["params"][2] },payloadJson["params"][3],0.8,1);
    //     DSTATUS("Step 1 over!EmergencyBrake for 2s\n");
    //     flightSample->emergencyBrake();
    //     sleep(20);
    //     flightSample->moveByPositionOffset((FlightSample::Vector3f){ payloadJson["params"][4], payloadJson["params"][5], payloadJson["params"][6] },payloadJson["params"][7],0.8,1);
    //     DSTATUS("Step 2 over!EmergencyBrakefor 2s\n");
    //     flightSample->emergencyBrake();
    //     sleep(20);
    //     flightSample->moveByPositionOffset((FlightSample::Vector3f){ payloadJson["params"][8], payloadJson["params"][9], payloadJson["params"][10] },payloadJson["params"][11],0.8,1);
    //     DSTATUS("Step 3 over!EmergencyBrake for 2s\n");
    //     flightSample->emergencyBrake();
    //     sleep(2);
    //     flightSample->moveByPositionOffset((FlightSample::Vector3f){ payloadJson["params"][12], payloadJson["params"][13], payloadJson["params"][14] },payloadJson["params"][15],0.8,1);
    //     DSTATUS("Step 4 over!EmergencyBrake for 2s\n");
    //     flightSample->emergencyBrake();
    //     sleep(2);
    //     // flightSample->monitoredLanding();
    //   }


  }

  continuousThread.join();
  // ret =
  // vehicle->flightController->releaseJoystickCtrlAuthoritySync(functionTimeout);
  vehicle->flightController->releaseJoystickCtrlAuthorityAsync(
    ReleaseJoystickCtrlAuthorityCB, nullptr, functionTimeout, 2);
  delete flightSample;
  return 0;
}
