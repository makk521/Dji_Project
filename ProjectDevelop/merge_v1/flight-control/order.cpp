
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


using json = nlohmann::json;

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;


json getCurrentDateTime(json& jsonObj, const std::string& key) {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&currentTime);
    std::stringstream ss;
    ss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    jsonObj[key]["time"] = ss.str();
    return jsonObj;
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

//被弃用了
// json structToJson(json& jsonObj, const LB2RcFullRawData& obj, const std::string& key)
// {
//     jsonObj[key]["roll"] = obj.roll;
//     jsonObj[key]["pitch"] = obj.pitch;
//     jsonObj[key]["yaw"] = obj.yaw;
//     jsonObj[key]["throttle"] = obj.throttle;
//     jsonObj[key]["mode"] = obj.mode;
//     jsonObj[key]["gear"] = obj.gear;
//     jsonObj[key]["camera"] = obj.camera;
//     jsonObj[key]["video"] = obj.video;
//     jsonObj[key]["videoPause"] = obj.videoPause;
//     jsonObj[key]["goHome"] = obj.goHome;
//     jsonObj[key]["leftWheel"] = obj.leftWheel;
//     jsonObj[key]["rightWheelButton"] = obj.rightWheelButton;
//     jsonObj[key]["rcC1"] = obj.rcC1;
//     jsonObj[key]["rcC2"] = obj.rcC2;
//     jsonObj[key]["rcD1"] = obj.rcD1;
//     jsonObj[key]["rcD2"] = obj.rcD2;
//     jsonObj[key]["rcD3"] = obj.rcD3;
//     jsonObj[key]["rcD4"] = obj.rcD4;
//     jsonObj[key]["rcD5"] = obj.rcD5;
//     jsonObj[key]["rcD6"] = obj.rcD6;
//     jsonObj[key]["rcD7"] = obj.rcD7;
//     jsonObj[key]["rcD8"] = obj.rcD8;
//     return jsonObj;
// }

// json structToJson(json& jsonObj, const SBUSFullRawData& obj, const std::string& key)
// {
//     jsonObj[key]["data"] = obj.data;
//     jsonObj[key]["reserved"] = obj.reserved;
//     return jsonObj;
// }

// json structToJson(json& jsonObj, const RCFullRawData& obj, const std::string& key)
// {
//     if (key == "lb2") {
//         jsonObj[key] = structToJson(jsonObj[key], obj.lb2, "LB2RcFullRawData");
//     } else if (key == "sbus") {
//         jsonObj[key] = structToJson(jsonObj[key], obj.sbus, "SBUSFullRawData");
//     }
//     return jsonObj;
// }
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


void
getValueFromTypeMap(const double& value)
{
  std::cout << "float64_t:" << value << '\n';
}

void
getValueFromTypeMap(const float32_t& value)
{
  std::cout << "float32_t:" << value << '\n';
}

void
getValueFromTypeMap(const uint32_t& value)
{
  std::cout << value << '\n';
}

void
getValueFromTypeMap(const int16_t& value)
{
  std::cout << value << '\n';
}

void getValueFromTypeMap(const uint8_t &value)
{
    std::cout << static_cast<int>(value) << '\n';
}

void
getValueFromTypeMap(const uint16_t& value)
{
  std::cout << value << '\n';
}


void
getValueFromTypeMap(const Quaternion& value)
{
  std::cout << "Quaternion: w=" << value.q0 << ", x=" << value.q1
            << ", y=" << value.q2 << ", z=" << value.q3 << '\n';
}

void
getValueFromTypeMap(const Vector3f& value)
{
  std::cout << "Vector3f: x=" << value.x << ", y=" << value.y
            << ", z=" << value.z << '\n';
}

void
getValueFromTypeMap(const VelocityInfo& value)
{
  std::cout << "VelocityInfo: GPS Health="
            << static_cast<int>(value.health)
            << '\n';
}

void
getValueFromTypeMap(const Velocity& value)
{
  getValueFromTypeMap(value.data);
  getValueFromTypeMap(value.info);
}

void
getValueFromTypeMap(const GPSFused& value)
{
  std::cout << "GPSFused: Longitude=" << value.longitude
            << ", Latitude=" << value.latitude
            << ", Altitude=" << value.altitude
            << ", Visible Satellites=" << value.visibleSatelliteNumber << '\n';
}

void
getValueFromTypeMap(const Vector3d& value)
{
  std::cout << "Vector3d: x=" << value.x << ", y=" << value.y
            << ", z=" << value.z << '\n';
}

void
getValueFromTypeMap(const GPSDetail& value)
{
  std::cout << "GPSDetail: HDOP=" << value.hdop << ", PDOP=" << value.pdop
            << ", Fix=" << value.fix << ", GNSS Status=" << value.gnssStatus
            << ", HACC=" << value.hacc << ", SACC=" << value.sacc
            << ", Used GPS Satellites=" << value.usedGPS
            << ", Used GLONASS Satellites=" << value.usedGLN
            << ", Total Satellites=" << value.NSV
            << ", GPS Data Counter=" << value.GPScounter << '\n';
}

void
getValueFromTypeMap(const PositionData& value)
{
  std::cout << "PositionData: Longitude=" << value.longitude
            << ", Latitude=" << value.latitude << ", HFSL=" << value.HFSL
            << '\n';
}

void
getValueFromTypeMap(const Mag& value)
{
  std::cout << "Mag: X=" << value.x << ", Y=" << value.y << ", Z=" << value.z
            << '\n';
}

void
getValueFromTypeMap(const RC& value)
{
  std::cout << "RC: Roll=" << value.roll << ", Pitch=" << value.pitch
            << ", Yaw=" << value.yaw << ", Throttle=" << value.throttle
            << ", Mode=" << value.mode << ", Gear=" << value.gear << '\n';
}

void
getValueFromTypeMap(const GimbalStatus& value)
{
  std::cout << "GimbalStatus: MountStatus: " << value.mountStatus
            << ", IsBusy: " << value.isBusy
            << ", PitchLimited: " << value.pitchLimited
            << ", RollLimited: " << value.rollLimited
            << ", YawLimited: " << value.yawLimited
            << ", Calibrating: " << value.calibrating
            << ", PrevCalibrationResult: " << value.prevCalibrationgResult
            << ", InstalledDirection: " << value.installedDirection
            << ", DisabledMVO: " << value.disabled_mvo
            << ", GearShowUnable: " << value.gear_show_unable
            << ", GyroFault: " << value.gyroFalut
            << ", ESCPitchStatus: " << value.escPitchStatus
            << ", ESCRollStatus: " << value.escRollStatus
            << ", ESCYawStatus: " << value.escYawStatus
            << ", DroneDataRecv: " << value.droneDataRecv
            << ", InitUnfinished: " << value.initUnfinished
            << ", FWUpdating: " << value.FWUpdating << '\n';
}

void
getValueFromTypeMap(const Battery& value)
{
  std::cout << "Battery: Capacity=" << value.capacity
            << ", Voltage=" << value.voltage << ", Current=" << value.current
            << ", Percentage=" << static_cast<int>(value.percentage) << '\n';
}

void
getValueFromTypeMap(const SDKInfo& value)
{
  std::cout << "SDKInfo: ControlMode: " << static_cast<int>(value.controlMode)
            << ", DeviceStatus: " << static_cast<int>(value.deviceStatus)
            << ", FlightStatus: " << static_cast<int>(value.flightStatus)
            << ", VRCStatus: " << static_cast<int>(value.vrcStatus) << '\n';
}

void
getValueFromTypeMap(const SyncTimestamp& value)
{
  std::cout << "SyncTimestamp: Time2p5ms=" << value.time2p5ms
            << ", Time1ns=" << value.time1ns
            << ", ResetTime2p5ms=" << value.resetTime2p5ms
            << ", Index=" << value.index
            << ", Flag=" << static_cast<int>(value.flag) << '\n';
}

void
getValueFromTypeMap(const HardSyncData& value)
{
  getValueFromTypeMap(value.ts);
  getValueFromTypeMap(value.q);
  getValueFromTypeMap(value.a);
  getValueFromTypeMap(value.w);
}

void
getValueFromTypeMap(const LB2RcFullRawData& value)
{
  std::cout << "LB2RcFullRawData:" << value.roll << ' ' << value.pitch << ' '
            << value.yaw << ' ' << value.throttle << ' ' << value.mode << ' '
            << value.gear << ' ' << value.camera << ' ' << value.video << ' '
            << value.videoPause << ' ' << value.goHome << ' ' << value.leftWheel
            << ' ' << value.rightWheelButton << ' ' << value.rcC1 << ' '
            << value.rcC2 << ' ' << value.rcD1 << ' ' << value.rcD2 << ' '
            << value.rcD3 << ' ' << value.rcD4 << ' ' << value.rcD5 << ' '
            << value.rcD6 << ' ' << value.rcD7 << ' ' << value.rcD8 << '\n';
}

void
getValueFromTypeMap(const SBUSFullRawData& value)
{
  std::cout << "SBUSFullRawData:";
  for (int i = 0; i < SDK_SBUS_CHANNEL_NUM; ++i)
  {
    std::cout << ' ' << value.data[i];
  }
  for (int i = 0; i < (SDK_LB2_CHANNEL_NUM - SDK_SBUS_CHANNEL_NUM); ++i)
  {
    std::cout << ' ' << value.reserved[i];
  }
  std::cout << '\n';
}

void
getValueFromTypeMap(const RCFullRawData& value)
{
  getValueFromTypeMap(value.lb2);
  getValueFromTypeMap(value.sbus);
}

void
getValueFromTypeMap(const RCWithFlagData& value)
{
  std::cout << "RCWithFlagData: " << value.pitch << ' ' << value.roll << ' '
            << value.yaw << ' ' << value.throttle << '\n';
  std::cout << "Flags: logicConnected="
            << static_cast<int>(value.flag.logicConnected)
            << ", skyConnected=" << static_cast<int>(value.flag.skyConnected)
            << ", groundConnected="
            << static_cast<int>(value.flag.groundConnected)
            << ", appConnected=" << static_cast<int>(value.flag.appConnected)
            << '\n';
}

void
getValueFromTypeMap(const ESCStatusIndividual& value)
{
  std::cout << "Current: " << value.current << ", Speed: " << value.speed
            << ", Voltage: " << value.voltage
            << ", Temperature: " << value.temperature
            << ", Stall: " << value.stall << ", Empty: " << value.empty
            << ", Unbalanced: " << value.unbalanced
            << ", ESC Disconnected: " << value.escDisconnected
            << ", Temperature High: " << value.temperatureHigh << '\n';
}

void
getValueFromTypeMap(const EscData& value)
{
  std::cout << "EscData: ";
  for (int i = 0; i < MAX_ESC_NUM; ++i)
  {
    std::cout << "ESC " << i + 1 << " - ";
    getValueFromTypeMap(value.esc[i]);
  }
}

void
getValueFromTypeMap(const RTKConnectStatus& value)
{
  std::cout << "RTKConnectStatus: RTK Connected: " << value.rtkConnected
            << '\n';
}

// void
// getValueFromTypeMap(const GimbalControlMode& value)
// {
//   std::cout << "GimbalControlMode: " << static_cast<int>(value) << '\n';
// }

void
getValueFromTypeMap(const FlightAnomaly& value)
{
  std::cout << "FlightAnomaly: "
            << "Impact in Air: " << value.impactInAir
            << ", Random Fly: " << value.randomFly
            << ", Height Ctrl Fail: " << value.heightCtrlFail
            << ", Roll Pitch Ctrl Fail: " << value.rollPitchCtrlFail
            << ", Yaw Ctrl Fail: " << value.yawCtrlFail
            << ", Aircraft Is Falling: " << value.aircraftIsFalling
            << ", Strong Wind Level 1: " << value.strongWindLevel1
            << ", Strong Wind Level 2: " << value.strongWindLevel2
            << ", Compass Installation Error: "
            << value.compassInstallationError
            << ", IMU Installation Error: " << value.imuInstallationError
            << ", ESC Temperature High: " << value.escTemperatureHigh
            << ", At Least One ESC Disconnected: "
            << value.atLeastOneEscDisconnected
            << ", GPS Yaw Error: " << value.gpsYawError << '\n';
}

void
getValueFromTypeMap(const LocalPositionVO& value)
{
  std::cout << "LocalPositionVO: "
            << "X: " << value.x << ", Y: " << value.y << ", Z: " << value.z
            << ", X Health: " << static_cast<int>(value.xHealth)
            << ", Y Health: " << static_cast<int>(value.yHealth)
            << ", Z Health: " << static_cast<int>(value.zHealth) << '\n';
}

void
getValueFromTypeMap(const DJI::OSDK::Telemetry::RelativePosition& value)
{
  std::cout << "RelativePosition: "
            << "Down: " << value.down << ", Front: " << value.front
            << ", Right: " << value.right << ", Back: " << value.back
            << ", Left: " << value.left << ", Up: " << value.up
            << ", Down Health: " << static_cast<int>(value.downHealth)
            << ", Front Health: " << static_cast<int>(value.frontHealth)
            << ", Right Health: " << static_cast<int>(value.rightHealth)
            << ", Back Health: " << static_cast<int>(value.backHealth)
            << ", Left Health: " << static_cast<int>(value.leftHealth)
            << ", Up Health: "
            << static_cast<int>(value.upHealth)
            << '\n';
}

void
getValueFromTypeMap(const HomeLocationSetStatus& value)
{
  std::cout << "HomeLocationSetStatus: Status: "
            << static_cast<int>(value.status) << '\n';
}

void
getValueFromTypeMap(const HomeLocationData& value)
{
  std::cout << "HomeLocationData: Latitude: " << value.latitude
            << ", Longitude: " << value.longitude << '\n';
}

#define SDK_M210_GIMBAL_MAX_NUM 2
#define SDK_M300_GIMBAL_MAX_NUM 3
void
getValueFromTypeMap(const GimbalSingleData& value)
{
  std::cout << "Pitch: " << value.pitch << ", Roll: " << value.roll
            << ", Yaw: " << value.yaw << ", Status: " << value.status
            << ", Mode: " << static_cast<int>(value.mode) << '\n';
}
void
getValueFromTypeMap(const GimbalDualData& value)
{
  for (int i = 0; i < SDK_M210_GIMBAL_MAX_NUM; ++i)
  {
    std::cout << "Gimbal " << i + 1 << " Data: ";
    getValueFromTypeMap(value.gbData[i]);
  }
}

void
getValueFromTypeMap(const GimbalThreeData& value)
{
  for (int i = 0; i < SDK_M300_GIMBAL_MAX_NUM; ++i)
  {
    std::cout << "Gimbal " << i + 1 << " Data: ";
    getValueFromTypeMap(value.gbData[i]);
  }
}

// // std::mutex mtx;
// // std::condition_variable cv;
// // bool monitoredLandingFinished = false;
// extern std::mutex mtx;
// extern std::condition_variable cv;
// extern bool monitoredLandingFinished;
// // bool shouldExit = false; 
// // bool isArmed = false;
// // bool isDisarmed = false;
// // bool isFlying = false;
extern bool isArmed; 
// extern bool isDisarmed;
extern bool isFlying;
// void printStatus() {
//     while (true) {
//         std::unique_lock<std::mutex> lock(mtx);
//         cv.wait(lock, [] { return monitoredLandingFinished; });

//         std::cout << "isArmed = " << isArmed << " isDisarmed = " << isDisarmed << " isFlying = " << isFlying << std::endl;
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }
// }



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





  while (1)
  {
    std::cout << "cmdType:\n";
    // 0上锁、1解锁、2起飞、3降落、4指点飞行、5返航、6设置返航高度、7紧急制动两秒

    DataPack value = commProcPriorityQueue.pop();
    std::string payloadString = value.payload;
    json payloadJson = json::parse(payloadString);
    
    // int cmdType;
    // std::cin >> cmdType;
    //switch (std::stoi(std::string(payloadJson["commandNum"])))
    // int commandNum = std::stoi(payloadJson["commandNum"].get<std::string>());
    int commandNum = payloadJson["commandNum"].get<int>();
    switch (commandNum)
    {
      case 0:
      {
        // 上锁
        std::cout << "dis arm\n";
        vehicle->control->disArmMotors();
        isArmed = false;
        isFlying = false;
        break;
      }
      case 1:
      {
        // 解锁
        std::cout << "arm\n";
        vehicle->control->armMotors();
        isArmed = false;
        isFlying = true;
        break;
      }
      case 2:
      {
        // 起飞
        std::cout << "takeoff\n";
        flightSample->monitoredTakeoff();
        break;
      }
      case 3:
      {
        // 降落
        std::cout << "land\n";
        flightSample->monitoredLanding();
        break;
      }
      case 4:
      {
        // 指点飞行
        std::cout << "move:please input x,y,z,yaw offset\n";
        std::cout << payloadJson["params"];
        float nums[4];
       // int params[] = payloadJson["params"].get<std::vector<int>>();
       flightSample->moveByPositionOffset(
         (FlightSample::Vector3f){ payloadJson["params"][0], payloadJson["params"][1], payloadJson["params"][2] },
         payloadJson["params"][3],
         0.8,
         1);
        break;
      }
      case 5:
      {
        // 返航
        std::cout << "go home\n";
        flightSample->goHomeAndConfirmLanding();
        break;
      }
      case 6:
      {
        // 设置返航高度
        std::cout << "set go home altitude:please input altitude\n";
        float height;
        std::cin >> height;
        flightSample->setGoHomeAltitude(height);
        break;
      }

      case 7:
      {
        flightSample->emergencyBrake();
        sleep(2);
        break;
      }

      // case 44:
      // {
      //   // 指点飞行
      //   // std::atomic<bool> shouldExit = false; 
      //   std::cout << "move:please input x,y,z,yaw offset\n";
      //   std::thread threadA([flightSample]() {flightSample->moveByPositionOffset((FlightSample::Vector3f){  3, 3, 3 },1,0.8,1);});
      //   std::thread threadB(printStatus); // 创建打印状态的线程

      //   threadA.join();
      //   // shouldExit = true; 
      //   threadB.join();
      //   break;
      // }



      case 55:
      {
        // 创建一个 promise 和一个 future 来获取函数的执行状态
        std::promise<bool> promiseResult;
        std::future<bool> futureResult = promiseResult.get_future();

        // 在线程中执行函数，并将执行结果通过 promise 传递给 future
        std::thread threadA([flightSample](std::promise<bool>& promise) {
            bool result = flightSample->moveByPositionOffset((FlightSample::Vector3f){ 3, 3, 3 },1,0.8,1);
            promise.set_value(result); // 将函数执行结果传递给 promise
        }, std::ref(promiseResult));

        // 等待函数执行完成，并获取执行结果
        std::future_status status = futureResult.wait_for(std::chrono::seconds(0));
        while (status != std::future_status::ready) {
            std::cout << "isArmed = " << isArmed <<" isFlying = " << isFlying << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // 每隔一秒检查一次状态
            status = futureResult.wait_for(std::chrono::seconds(0));
        }

        // 获取函数的执行结果
        // bool executionResult = futureResult.get();
        std::cout << "isArmed = " << isArmed <<  " isFlying = " << isFlying << std::endl;



        // flightSample->teardownSubscription(pkgIndex1,timeout);
        // flightSample->teardownSubscription(pkgIndex2,timeout);
        // // flightSample->teardownSubscription(pkgIndex3,timeout);
        // // flightSample->teardownSubscription(pkgIndex4,timeout);
        // flightSample->teardownSubscription(pkgIndex5,timeout);

        threadA.join();
        break;
      }



      case 66:
      {
        std::cout << "get value\n";
        for (int i = 0 ;i<20;i++){
          // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ESC_DATA>());
          // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>());
          getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>());
          getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>());
          getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>());
          getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>());
          getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_TIME>());
          getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_POSITION>());
        }
        break;
      }

      case 33:
      {
        for(int i= 0;i<10;i++){
          json mergedJson;
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_VELOCITY>(), "TOPIC_GPS_VELOCITY");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>(), "TOPIC_RTK_VELOCITY");
          structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_GROUND>(), "TOPIC_ACCELERATION_GROUND");
          std::cout << mergedJson.dump() << '\n';
        }
        break;
      }



      case 77:
      {
        std::thread threadA([flightSample]() {flightSample->moveByPositionOffset((FlightSample::Vector3f){ 10, 10, 10 },1,0.8,1);});
        std::thread threadB([=]() {
            auto last_iteration_time = std::chrono::steady_clock::now();
            for (int i = 0; i < 100; ++i) {
                // 启动异步任务执行操作
                json mergedJson;
                std::future<void> future = std::async(std::launch::async, [&](){
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_VELOCITY>(), "TOPIC_GPS_VELOCITY");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>(), "TOPIC_RTK_VELOCITY");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_GROUND>(), "TOPIC_ACCELERATION_GROUND");
                std::cout << mergedJson.dump() << '\n';
                });

                // 计算已经过去的时间
                auto current_time = std::chrono::steady_clock::now();
                auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_iteration_time).count();
                // 计算需要等待的时间，确保在每次迭代中等待0.1秒
                auto wait_time = 100 - elapsed_time;
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

        threadA.join();
        threadB.join();

        std::cout << "Both threads have finished execution." << std::endl;
        break;
      }


      case 88:
      {
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
        break;
      }


      case 8:
      {

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


        auto last_iteration_time = std::chrono::steady_clock::now();
        for (int i = 0; i < 5; ++i) {
            // 启动异步任务执行操作
            json mergedJson;
            std::future<void> future = std::async(std::launch::async, [&](){
            getCurrentDateTime(mergedJson,"TIME_NOW");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DATE>(), "TOPIC_GPS_DATE");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_TIME>(), "TOPIC_GPS_TIME");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_POSITION>(), "TOPIC_GPS_POSITION");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_VELOCITY>(), "TOPIC_GPS_VELOCITY");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>(), "TOPIC_GPS_DETAILS");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_POSITION>(), "TOPIC_RTK_POSITION");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>(), "TOPIC_RTK_VELOCITY");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_YAW>(), "TOPIC_RTK_YAW");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_POSITION_INFO>(), "TOPIC_RTK_POSITION_INFO");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_YAW_INFO>(), "TOPIC_RTK_YAW_INFO");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>(), "TOPIC_BATTERY_INFO");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>(), "TOPIC_CONTROL_DEVICE");

            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_QUATERNION>(), "TOPIC_QUATERNION");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_GROUND>(), "TOPIC_ACCELERATION_GROUND");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_BODY>(), "TOPIC_ACCELERATION_BODY");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_RAW>(), "TOPIC_ACCELERATION_RAW");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_VELOCITY>(), "TOPIC_VELOCITY");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_FUSIONED>(), "TOPIC_ANGULAR_RATE_FUSIONED");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_RAW>(), "TOPIC_ANGULAR_RATE_RAW");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>(), "TOPIC_ALTITUDE_FUSIONED");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>(), "TOPIC_ALTITUDE_BAROMETER");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>(), "TOPIC_HEIGHT_FUSION");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_FUSED>(), "TOPIC_GPS_FUSED");

            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_COMPASS>(), "TOPIC_COMPASS");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RC>(), "TOPIC_RC");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GIMBAL_ANGLES>(), "TOPIC_GIMBAL_ANGLES");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GIMBAL_STATUS>(), "TOPIC_GIMBAL_STATUS");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>(), "TOPIC_STATUS_FLIGHT");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>(), "TOPIC_STATUS_DISPLAYMODE");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_LANDINGGEAR>(), "TOPIC_STATUS_LANDINGGEAR");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_MOTOR_START_ERROR>(), "TOPIC_STATUS_MOTOR_START_ERROR");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HARD_SYNC>(), "TOPIC_HARD_SYNC");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_SIGNAL_LEVEL>(), "TOPIC_GPS_SIGNAL_LEVEL");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_CONTROL_LEVEL>(), "TOPIC_GPS_CONTROL_LEVEL");

            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RC_WITH_FLAG_DATA>(), "TOPIC_RC_WITH_FLAG_DATA");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ESC_DATA>(), "TOPIC_ESC_DATA");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_CONNECT_STATUS>(), "TOPIC_RTK_CONNECT_STATUS");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GIMBAL_CONTROL_MODE>(), "TOPIC_GIMBAL_CONTROL_MODE");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_FLIGHT_ANOMALY>(), "TOPIC_FLIGHT_ANOMALY");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_POSITION_VO>(), "TOPIC_POSITION_VO");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_AVOID_DATA>(), "TOPIC_AVOID_DATA");

            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>(), "TOPIC_ALTITUDE_OF_HOMEPOINT");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HOME_POINT_SET_STATUS>(), "TOPIC_HOME_POINT_SET_STATUS");
            structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HOME_POINT_INFO>(), "TOPIC_HOME_POINT_INFO");
            std::cout << mergedJson.dump() << '\n';
            });
            // 计算已经过去的时间
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_iteration_time).count();
            // 计算需要等待的时间，确保在每次迭代中等待0.1秒
            auto wait_time = 100 - elapsed_time;
            if (wait_time > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
            }
            else{
              std::cout <<"time false\n";
            }
            last_iteration_time = std::chrono::steady_clock::now(); // 更新上一次迭代的时间
            std::cout<<"一轮结束\n";
        }

        // for (int i = 0; i < 10; ++i) {
        //     json mergedJson;
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DATE>(), "TOPIC_GPS_DATE");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_TIME>(), "TOPIC_GPS_TIME");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_POSITION>(), "TOPIC_GPS_POSITION");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_VELOCITY>(), "TOPIC_GPS_VELOCITY");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>(), "TOPIC_GPS_DETAILS");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_POSITION>(), "TOPIC_RTK_POSITION");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>(), "TOPIC_RTK_VELOCITY");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_YAW>(), "TOPIC_RTK_YAW");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_POSITION_INFO>(), "TOPIC_RTK_POSITION_INFO");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_YAW_INFO>(), "TOPIC_RTK_YAW_INFO");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>(), "TOPIC_BATTERY_INFO");
        //     // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>(), "TOPIC_CONTROL_DEVICE");

        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_QUATERNION>(), "TOPIC_QUATERNION");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_GROUND>(), "TOPIC_ACCELERATION_GROUND");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_BODY>(), "TOPIC_ACCELERATION_BODY");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_RAW>(), "TOPIC_ACCELERATION_RAW");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_VELOCITY>(), "TOPIC_VELOCITY");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_FUSIONED>(), "TOPIC_ANGULAR_RATE_FUSIONED");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_RAW>(), "TOPIC_ANGULAR_RATE_RAW");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>(), "TOPIC_ALTITUDE_FUSIONED");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>(), "TOPIC_ALTITUDE_BAROMETER");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>(), "TOPIC_HEIGHT_FUSION");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_FUSED>(), "TOPIC_GPS_FUSED");

        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_COMPASS>(), "TOPIC_COMPASS");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RC>(), "TOPIC_RC");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GIMBAL_ANGLES>(), "TOPIC_GIMBAL_ANGLES");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GIMBAL_STATUS>(), "TOPIC_GIMBAL_STATUS");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>(), "TOPIC_STATUS_FLIGHT");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>(), "TOPIC_STATUS_DISPLAYMODE");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_LANDINGGEAR>(), "TOPIC_STATUS_LANDINGGEAR");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_MOTOR_START_ERROR>(), "TOPIC_STATUS_MOTOR_START_ERROR");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HARD_SYNC>(), "TOPIC_HARD_SYNC");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_SIGNAL_LEVEL>(), "TOPIC_GPS_SIGNAL_LEVEL");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_CONTROL_LEVEL>(), "TOPIC_GPS_CONTROL_LEVEL");

        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RC_WITH_FLAG_DATA>(), "TOPIC_RC_WITH_FLAG_DATA");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ESC_DATA>(), "TOPIC_ESC_DATA");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_CONNECT_STATUS>(), "TOPIC_RTK_CONNECT_STATUS");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GIMBAL_CONTROL_MODE>(), "TOPIC_GIMBAL_CONTROL_MODE");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_FLIGHT_ANOMALY>(), "TOPIC_FLIGHT_ANOMALY");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_POSITION_VO>(), "TOPIC_POSITION_VO");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_AVOID_DATA>(), "TOPIC_AVOID_DATA");

        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>(), "TOPIC_ALTITUDE_OF_HOMEPOINT");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HOME_POINT_SET_STATUS>(), "TOPIC_HOME_POINT_SET_STATUS");
        //     structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HOME_POINT_INFO>(), "TOPIC_HOME_POINT_INFO");
        //     std::cout << mergedJson.dump() << '\n';
        //     std::cout<<"一轮结束\n";
        // }
        flightSample->teardownSubscription(pkgIndex1,timeout);
        flightSample->teardownSubscription(pkgIndex2,timeout);
        flightSample->teardownSubscription(pkgIndex3,timeout);
        flightSample->teardownSubscription(pkgIndex4,timeout);
        flightSample->teardownSubscription(pkgIndex5,timeout);
        break;
      }

      case 9:
      {
        int freq1 = 5;
        int pkgIndex1 = 1;
        int topicSize1 = sizeof(topicList1) / sizeof(topicList1[0]);
        flightSample->setUpSubscription(pkgIndex1, freq1, topicList1, topicSize1, timeout);

        int freq2 = 10;
        int pkgIndex2 = 2;
        int topicSize2 = sizeof(topicList2) / sizeof(topicList2[0]);
        flightSample->setUpSubscription(pkgIndex2, freq2, topicList2, topicSize2, timeout);

        // int freq3 = 10;
        // int pkgIndex3 = 3;
        // int topicSize3 = sizeof(topicList3) / sizeof(topicList3[0]);
        // flightSample->setUpSubscription(pkgIndex3, freq3, topicList3, topicSize3, timeout);        

        // int freq4 = 10;
        // int pkgIndex4 = 4;
        // int topicSize4 = sizeof(topicList4) / sizeof(topicList4[0]);
        // flightSample->setUpSubscription(pkgIndex4, freq4, topicList4, topicSize4, timeout);

        int freq5 = 1;
        int pkgIndex5 = 5;
        int topicSize5 = sizeof(topicList5) / sizeof(topicList5[0]);
        flightSample->setUpSubscription(pkgIndex5, freq5, topicList5, topicSize5, timeout);

        

        std::thread threadA([flightSample]() {flightSample->moveByPositionOffset((FlightSample::Vector3f){ 40, 40, 40 },1,0.8,1);});
        std::thread threadB([=]() {
            auto last_iteration_time = std::chrono::steady_clock::now();
            for (int i = 0; i < 100; ++i) {
                // 启动异步任务执行操作
                json mergedJson;
                std::future<void> future = std::async(std::launch::async, [&](){
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DATE>(), "TOPIC_GPS_DATE");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_TIME>(), "TOPIC_GPS_TIME");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_POSITION>(), "TOPIC_GPS_POSITION");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_VELOCITY>(), "TOPIC_GPS_VELOCITY");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>(), "TOPIC_GPS_DETAILS");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_POSITION>(), "TOPIC_RTK_POSITION");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>(), "TOPIC_RTK_VELOCITY");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_YAW>(), "TOPIC_RTK_YAW");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_POSITION_INFO>(), "TOPIC_RTK_POSITION_INFO");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_YAW_INFO>(), "TOPIC_RTK_YAW_INFO");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>(), "TOPIC_BATTERY_INFO");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>(), "TOPIC_CONTROL_DEVICE");

                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_QUATERNION>(), "TOPIC_QUATERNION");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_GROUND>(), "TOPIC_ACCELERATION_GROUND");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_BODY>(), "TOPIC_ACCELERATION_BODY");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ACCELERATION_RAW>(), "TOPIC_ACCELERATION_RAW");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_VELOCITY>(), "TOPIC_VELOCITY");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_FUSIONED>(), "TOPIC_ANGULAR_RATE_FUSIONED");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_RAW>(), "TOPIC_ANGULAR_RATE_RAW");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>(), "TOPIC_ALTITUDE_FUSIONED");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>(), "TOPIC_ALTITUDE_BAROMETER");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>(), "TOPIC_HEIGHT_FUSION");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_FUSED>(), "TOPIC_GPS_FUSED");

                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_COMPASS>(), "TOPIC_COMPASS");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RC>(), "TOPIC_RC");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GIMBAL_ANGLES>(), "TOPIC_GIMBAL_ANGLES");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GIMBAL_STATUS>(), "TOPIC_GIMBAL_STATUS");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>(), "TOPIC_STATUS_FLIGHT");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>(), "TOPIC_STATUS_DISPLAYMODE");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_LANDINGGEAR>(), "TOPIC_STATUS_LANDINGGEAR");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_STATUS_MOTOR_START_ERROR>(), "TOPIC_STATUS_MOTOR_START_ERROR");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HARD_SYNC>(), "TOPIC_HARD_SYNC");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_SIGNAL_LEVEL>(), "TOPIC_GPS_SIGNAL_LEVEL");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_CONTROL_LEVEL>(), "TOPIC_GPS_CONTROL_LEVEL");

                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RC_WITH_FLAG_DATA>(), "TOPIC_RC_WITH_FLAG_DATA");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ESC_DATA>(), "TOPIC_ESC_DATA");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_CONNECT_STATUS>(), "TOPIC_RTK_CONNECT_STATUS");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GIMBAL_CONTROL_MODE>(), "TOPIC_GIMBAL_CONTROL_MODE");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_FLIGHT_ANOMALY>(), "TOPIC_FLIGHT_ANOMALY");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_POSITION_VO>(), "TOPIC_POSITION_VO");
                // structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_AVOID_DATA>(), "TOPIC_AVOID_DATA");

                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>(), "TOPIC_ALTITUDE_OF_HOMEPOINT");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HOME_POINT_SET_STATUS>(), "TOPIC_HOME_POINT_SET_STATUS");
                structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_HOME_POINT_INFO>(), "TOPIC_HOME_POINT_INFO");
                std::cout << mergedJson.dump() << '\n';
                });

                // 计算已经过去的时间
                auto current_time = std::chrono::steady_clock::now();
                auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_iteration_time).count();
                // 计算需要等待的时间，确保在每次迭代中等待0.1秒
                auto wait_time = 100 - elapsed_time;
                if (wait_time > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
                }
                else{
                  std::cout <<"time false\n";
                }
                last_iteration_time = std::chrono::steady_clock::now(); // 更新上一次迭代的时间
                std::cout<<"一轮结束\n";
                
            }


            // flightSample->teardownSubscription(pkgIndex1,timeout);
            // flightSample->teardownSubscription(pkgIndex2,timeout);
            // // flightSample->teardownSubscription(pkgIndex3,timeout);
            // // flightSample->teardownSubscription(pkgIndex4,timeout);
            // flightSample->teardownSubscription(pkgIndex5,timeout);
        });


        threadA.join();
        threadB.join();
        flightSample->teardownSubscription(pkgIndex1,timeout);
        flightSample->teardownSubscription(pkgIndex2,timeout);
        // flightSample->teardownSubscription(pkgIndex3,timeout);
        // flightSample->teardownSubscription(pkgIndex4,timeout);
        flightSample->teardownSubscription(pkgIndex5,timeout);
        std::cout << "Both threads have finished execution." << std::endl;
        break;
      }




      case 10:
      {
        // 创建一个 promise 和一个 future 来获取函数的执行状态
        std::promise<bool> promiseResult;
        std::future<bool> futureResult = promiseResult.get_future();

        // 在线程中执行函数，并将执行结果通过 promise 传递给 future
        std::thread threadA([flightSample](std::promise<bool>& promise) {
            bool result = flightSample->moveByPositionOffset((FlightSample::Vector3f){ 3, 3, 3 },1,0.8,1);
            promise.set_value(result); // 将函数执行结果传递给 promise
        }, std::ref(promiseResult));

        // 等待函数执行完成，并获取执行结果
        std::future_status status = futureResult.wait_for(std::chrono::seconds(0));
        while (status != std::future_status::ready) {
            std::cout << "函数正在执行..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // 每隔一秒检查一次状态
            status = futureResult.wait_for(std::chrono::seconds(0));
        }

        // 获取函数的执行结果
        bool executionResult = futureResult.get();

        // 根据执行结果做相应的处理
        if (executionResult) {
            std::cout << "函数执行成功！" << std::endl;
        } else {
            std::cout << "函数执行失败或超时！" << std::endl;
        }

        // flightSample->teardownSubscription(pkgIndex1,timeout);
        // flightSample->teardownSubscription(pkgIndex2,timeout);
        // // flightSample->teardownSubscription(pkgIndex3,timeout);
        // // flightSample->teardownSubscription(pkgIndex4,timeout);
        // flightSample->teardownSubscription(pkgIndex5,timeout);

        threadA.join();
        break;
      }



      case 20:
      {
        int freq2 = 10;
        int pkgIndex2 = 2;
        int topicSize2 = sizeof(topicList2) / sizeof(topicList2[0]);
        flightSample->setUpSubscription(pkgIndex2, freq2, topicList2, topicSize2, timeout);

        // 创建一个 promise 和一个 future 来获取函数的执行状态
        std::promise<bool> promiseResult;
        std::future<bool> futureResult = promiseResult.get_future();

        // 在线程中执行函数，并将执行结果通过 promise 传递给 future
        std::thread threadA([flightSample](std::promise<bool>& promise) {
            bool result = flightSample->moveByPositionOffset((FlightSample::Vector3f){ 3, 3, 3 },1,0.8,1);
            promise.set_value(result); // 将函数执行结果传递给 promise
        }, std::ref(promiseResult));

        // 等待函数执行完成，并获取执行结果
        std::future_status status = futureResult.wait_for(std::chrono::seconds(0));
        while (status != std::future_status::ready) {
            std::cout << "函数正在执行..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // 每隔一秒检查一次状态
            status = futureResult.wait_for(std::chrono::seconds(0));
        }

        // 获取函数的执行结果
        bool executionResult = futureResult.get();

        // 根据执行结果做相应的处理
        if (executionResult) {
            std::cout << "函数执行成功！" << std::endl;
        } else {
            std::cout << "函数执行失败或超时！" << std::endl;
        }

        // flightSample->teardownSubscription(pkgIndex1,timeout);
        flightSample->teardownSubscription(pkgIndex2,timeout);
        // // flightSample->teardownSubscription(pkgIndex3,timeout);
        // // flightSample->teardownSubscription(pkgIndex4,timeout);
        // flightSample->teardownSubscription(pkgIndex5,timeout);

        threadA.join();
        break;
      }

      case 21:
      {
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

        // 创建一个 promise 和一个 future 来获取函数的执行状态
        std::promise<bool> promiseResult;
        std::future<bool> futureResult = promiseResult.get_future();

        // 在线程中执行函数，并将执行结果通过 promise 传递给 future
        std::thread threadA([flightSample](std::promise<bool>& promise) {
            bool result = flightSample->goHomeAndConfirmLanding();
            promise.set_value(result); // 将函数执行结果传递给 promise
        }, std::ref(promiseResult));

        // 等待函数执行完成，并获取执行结果
        std::future_status status = futureResult.wait_for(std::chrono::seconds(0));
        while (status != std::future_status::ready) {
            std::cout << "函数正在执行..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // 每隔一秒检查一次状态
            status = futureResult.wait_for(std::chrono::seconds(0));
        }

        // 获取函数的执行结果
        bool executionResult = futureResult.get();

        // 根据执行结果做相应的处理
        if (executionResult) {
            std::cout << "函数执行成功！" << std::endl;
        } else {
            std::cout << "函数执行失败或超时！" << std::endl;
        }

        flightSample->teardownSubscription(pkgIndex1,timeout);
        flightSample->teardownSubscription(pkgIndex2,timeout);
        flightSample->teardownSubscription(pkgIndex3,timeout);
        flightSample->teardownSubscription(pkgIndex4,timeout);
        flightSample->teardownSubscription(pkgIndex5,timeout);

        threadA.join();
        break;
      }



      case 30:
      {
        int freq2 = 10;
        int pkgIndex2 = 2;
        int topicSize2 = sizeof(topicList2) / sizeof(topicList2[0]);
        flightSample->setUpSubscription(pkgIndex2, freq2, topicList2, topicSize2, timeout);
        std::thread threadA([flightSample]() {flightSample->moveByPositionOffset((FlightSample::Vector3f){ 3, 3, 3 },1,0.8,1);});
        threadA.join();
        flightSample->teardownSubscription(pkgIndex2,timeout);
        break;
      }



      case 40:
      {
        flightSample->monitoredTakeoff();
        DSTATUS("Take off over!\n");

        flightSample->velocityAndYawRateCtrl((FlightSample::Vector3f){0, 0, 5.0}, 0, 2000);
        DSTATUS("Step 1 over!EmergencyBrake for 2s\n");
        flightSample->emergencyBrake();
        sleep(20);
        flightSample->velocityAndYawRateCtrl((FlightSample::Vector3f){-1.5, 2, 0}, 0, 2000);
        DSTATUS("Step 2 over!EmergencyBrakefor 2s\n");
        flightSample->emergencyBrake();
        sleep(20);
        flightSample->velocityAndYawRateCtrl((FlightSample::Vector3f){3, 0, 0}, 0, 2500);
        DSTATUS("Step 3 over!EmergencyBrake for 2s\n");
        flightSample->emergencyBrake();
        sleep(2);
        flightSample->velocityAndYawRateCtrl((FlightSample::Vector3f){-1.6, -2, 0}, 0, 2200);
        DSTATUS("Step 4 over!EmergencyBrake for 2s\n");
        flightSample->emergencyBrake();
        sleep(2);

        flightSample->monitoredLanding();
      }




      case 11:
      {
        std::cout << "get value\n";



        int freq = 1;
        int pkgIndex = 1;
        int topicSize = sizeof(topicList1) / sizeof(topicList1[0]);
        flightSample->setUpSubscription(pkgIndex, freq, topicList1, topicSize, timeout);
        
        auto last_iteration_time = std::chrono::steady_clock::now();
        for (int i = 0; i < 10; ++i) {
            // 启动异步任务执行两个操作
            json mergedJson;
            std::future<void> future = std::async(std::launch::async, [&](){
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DATE>(), "TOPIC_GPS_DATE");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_TIME>(), "TOPIC_GPS_TIME");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_POSITION>(), "TOPIC_GPS_POSITION");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_VELOCITY>(), "TOPIC_GPS_VELOCITY");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>(), "TOPIC_GPS_DETAILS");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_POSITION>(), "TOPIC_RTK_POSITION");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>(), "TOPIC_RTK_VELOCITY");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_YAW>(), "TOPIC_RTK_YAW");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_POSITION_INFO>(), "TOPIC_RTK_POSITION_INFO");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_RTK_YAW_INFO>(), "TOPIC_RTK_YAW_INFO");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>(), "TOPIC_BATTERY_INFO");
              structToJson(mergedJson, vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>(), "TOPIC_CONTROL_DEVICE");
            });
            // 计算已经过去的时间
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_iteration_time).count();
            // 计算需要等待的时间，确保在每次迭代中等待0.1秒
            auto wait_time = 100 - elapsed_time;
            if (wait_time > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
            }
            last_iteration_time = std::chrono::steady_clock::now(); // 更新上一次迭代的时间
            std::cout << mergedJson.dump() << '\n';
        }

        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_DATE>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_TIME>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_POSITION>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_VELOCITY>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_POSITION>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_YAW>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_POSITION_INFO>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_YAW_INFO>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_HOME_POINT_SET_STATUS>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_HOME_POINT_INFO>());



        // int pkgIndex = 1;
        // int topicSize = sizeof(topicList1) / sizeof(topicList1[0]);
        // flightSample->setUpSubscription(
        //   pkgIndex, freq, topicList1, topicSize, timeout);

        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_QUATERNION>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_ACCELERATION_GROUND>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_ACCELERATION_BODY>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_ACCELERATION_RAW>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_VELOCITY>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_FUSIONED>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_RAW>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_FUSED>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_DATE>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_TIME>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_POSITION>());

        //flightSample->teardownSubscription(pkgIndex);
        flightSample->teardownSubscription(pkgIndex,timeout);
        break;
      }

      case 12:
      {
        std::cout << "get value\n";
        int pkgIndex = 2;

        int topicSize = sizeof(topicList2) / sizeof(topicList2[0]);
        flightSample->setUpSubscription(
          pkgIndex, freq, topicList2, topicSize, timeout);

        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_QUATERNION>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ACCELERATION_GROUND>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ACCELERATION_BODY>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ACCELERATION_RAW>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_VELOCITY>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_FUSIONED>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_RAW>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_FUSED>());

        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_VELOCITY>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_POSITION>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_YAW>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_RTK_POSITION_INFO>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_COMPASS>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RC>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_GIMBAL_ANGLES>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_GIMBAL_STATUS>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_STATUS_LANDINGGEAR>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_STATUS_MOTOR_START_ERROR>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>());
        
        flightSample->teardownSubscription(pkgIndex,timeout);
        break;
      }

      case 13:
      {
        std::cout << "get value\n";
        int pkgIndex = 3;

        int topicSize = sizeof(topicList3) / sizeof(topicList3[0]);
        flightSample->setUpSubscription(
          pkgIndex, freq, topicList3, topicSize, timeout);
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_COMPASS>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RC>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GIMBAL_ANGLES>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GIMBAL_STATUS>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_STATUS_LANDINGGEAR>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_STATUS_MOTOR_START_ERROR>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_HARD_SYNC>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_SIGNAL_LEVEL>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_CONTROL_LEVEL>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_HARD_SYNC>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_GPS_SIGNAL_LEVEL>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_GPS_CONTROL_LEVEL>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_RC_FULL_RAW_DATA>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_RC_WITH_FLAG_DATA>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ESC_DATA>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_RTK_CONNECT_STATUS>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_GIMBAL_CONTROL_MODE>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_FLIGHT_ANOMALY>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_POSITION_VO>());
        // // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_AVOID_DATA>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_HOME_POINT_SET_STATUS>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_HOME_POINT_INFO>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_DUAL_GIMBAL_DATA>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_THREE_GIMBAL_DATA>());
        
        flightSample->teardownSubscription(pkgIndex,timeout);
        break;
      }


      case 14:
      {
        std::cout << "get value\n";
        int pkgIndex = 4;

        int topicSize = sizeof(topicList4) / sizeof(topicList4[0]);
        flightSample->setUpSubscription(
          pkgIndex, freq, topicList4, topicSize, timeout);
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RC_WITH_FLAG_DATA>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ESC_DATA>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_CONNECT_STATUS>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GIMBAL_CONTROL_MODE>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_FLIGHT_ANOMALY>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_POSITION_VO>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_AVOID_DATA>());

        // // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_HARD_SYNC>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_GPS_SIGNAL_LEVEL>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_GPS_CONTROL_LEVEL>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_RC_FULL_RAW_DATA>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_RC_WITH_FLAG_DATA>());
        // // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ESC_DATA>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_RTK_CONNECT_STATUS>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_GIMBAL_CONTROL_MODE>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_FLIGHT_ANOMALY>());
        // // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_POSITION_VO>());
        // getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_AVOID_DATA>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_HOME_POINT_SET_STATUS>());
        // getValueFromTypeMap(
        //   vehicle->subscribe->getValue<TOPIC_HOME_POINT_INFO>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_DUAL_GIMBAL_DATA>());
        // // getValueFromTypeMap(
        // //   vehicle->subscribe->getValue<TOPIC_THREE_GIMBAL_DATA>());
        
        flightSample->teardownSubscription(pkgIndex,timeout);
        break;
      }

      // case 100:
      // {

      //   // 设置返航高度
      //   std::cout << "get value\n";
      //   int pkgIndex = 100;
      //   int topicSize = sizeof(topicList10Hz) / sizeof(topicList10Hz[0]);
      //   flightSample->setUpSubscription(
      //     pkgIndex, freq, topicList10Hz, topicSize, timeout);


      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_QUATERNION>());
      //   /*
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ACCELERATION_GROUND>());
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ACCELERATION_BODY>());
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ACCELERATION_RAW>());
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_VELOCITY>());
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_FUSIONED>());
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_RAW>());
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>());
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>());
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>());
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>());
      //   */
      //   getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>());


      //   break;
      // }
      default:
        break;
    }
  }

  // ret =
  // vehicle->flightController->releaseJoystickCtrlAuthoritySync(functionTimeout);
  vehicle->flightController->releaseJoystickCtrlAuthorityAsync(
    ReleaseJoystickCtrlAuthorityCB, nullptr, functionTimeout, 2);
  delete flightSample;
  return 0;
}
