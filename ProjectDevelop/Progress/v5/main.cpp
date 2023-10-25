
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
// #include "flight_control_sample.hpp"
#include "dji_linux_helpers.hpp"
#include "flight_sample.hpp"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include "nlohmann/json.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>


using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;
using json = nlohmann::json;
using namespace std;

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;

    // œÚ∂”¡–÷–Õ∆ÀÕ ˝æ›
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(value);
        condition_variable.notify_one();
    }

    // ¥”∂”¡–÷–µØ≥ˆ ˝æ›
    T pop() {
        std::unique_lock<std::mutex> lock(mutex);
        condition_variable.wait(lock, [this] { return !queue.empty(); });
        T value = queue.front();
        queue.pop();
        return value;
    }

private:
    std::queue<T> queue;  // Use std::queue to store data
    std::mutex mutex;
    std::condition_variable condition_variable;
};

ThreadSafeQueue<int> sharedCommandQueue;


void postData(int clientSocket, sockaddr_in serverAddr){
    usleep(10000000);
    cout << "Post delay 10 seconds" << endl;
    // ¡¨Ω”µΩ∑˛ŒÒ∆˜
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        close(clientSocket);
        return ;
    }
    // ≥÷–¯∑¢ÀÕ ˝æ›
    const char* message = "Hello from client";
    while (true) {
        int bytesSent = send(clientSocket, message, strlen(message), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            break;
        }
        cout << "Sent data: " << message << endl;

        // ÃÌº”  µ±µƒ—” ±£¨“‘øÿ÷∆∑¢ÀÕÀŸ¬ 
        usleep(3000000); // –›√ﬂ 1 √Î
    }
    
}

void receiveData(int serverSocket, sockaddr_in serverAddr, ThreadSafeQueue<int>& sharedQueue) {
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding");
        return ;
    }

    // º‡Ã˝¡¨Ω”
    if (listen(serverSocket, 5) == -1) {
        perror("Error listening");
        return ;
    }
    // Ω” ‹¡¨Ω”
    cout << "Server listening on port 5000..." << endl;
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Error accepting client connection");
        return;
    }
    
    cout << "Client connected" << endl;

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == -1) {
            perror("Error receiving data");
            break;
        }
        if (bytesReceived == 0) {
            cout << "Client disconnected" << endl;
            break;
        }
        cout << "Received: " << buffer << endl;
        sharedQueue.push(stoi(buffer));
        cout << "Push Success" << endl; 

        // ∑¢ÀÕœÏ”¶
        const char* response = "Hello from server";
        send(clientSocket, response, strlen(response), 0);
    }

    close(clientSocket);
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

// void getValueFromTypeMap(const uint8_t &value)
// {
//     std::cout << static_cast<int>(value) << '\n';
// }

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

void
getValueFromTypeMap(const GimbalControlMode& value)
{
  std::cout << "GimbalControlMode: " << static_cast<int>(value) << '\n';
}

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

void uavExecute(int argc, char** argv, ThreadSafeQueue<int>& sharedQueue){
	int functionTimeout = 1;
	  // Setup OSDK.
  LinuxSetup linuxEnvironment(argc, argv);
  Vehicle*   vehicle = linuxEnvironment.getVehicle();
  if (vehicle == NULL)
  {
    std::cout << "Vehicle not initialized, exiting.\n";
    return ;
  }

  // Obtain Control Authority
  // ErrorCode::ErrorCodeType ret =
  // vehicle->flightController->obtainJoystickCtrlAuthoritySync(functionTimeout);
  vehicle->flightController->obtainJoystickCtrlAuthorityAsync(
    ObtainJoystickCtrlAuthorityCB, nullptr, functionTimeout, 2);
  FlightSample* flightSample = new FlightSample(vehicle);

  int timeout  = 10;
  // int pkgIndex = 0;
  int freq     = 10;

  TopicName topicList1[] = {
    TOPIC_QUATERNION,         TOPIC_ACCELERATION_GROUND,
    TOPIC_ACCELERATION_BODY,  TOPIC_ACCELERATION_RAW,
    TOPIC_VELOCITY,           TOPIC_ANGULAR_RATE_FUSIONED,
    TOPIC_ANGULAR_RATE_RAW,   TOPIC_ALTITUDE_FUSIONED,
    TOPIC_ALTITUDE_BAROMETER, TOPIC_ALTITUDE_OF_HOMEPOINT,
    TOPIC_HEIGHT_FUSION,      TOPIC_GPS_FUSED,
    TOPIC_GPS_DATE,           TOPIC_GPS_TIME,
    TOPIC_GPS_POSITION
  };

  TopicName topicList2[] = {
    TOPIC_GPS_VELOCITY,
    TOPIC_GPS_DETAILS,
    TOPIC_RTK_POSITION,
    TOPIC_RTK_VELOCITY,
    TOPIC_RTK_YAW,
    TOPIC_RTK_POSITION_INFO,
    TOPIC_RTK_YAW_INFO,
    TOPIC_COMPASS,
    TOPIC_RC,
    TOPIC_GIMBAL_ANGLES,
    TOPIC_GIMBAL_STATUS,
    TOPIC_STATUS_FLIGHT,
    TOPIC_STATUS_DISPLAYMODE,
    TOPIC_STATUS_LANDINGGEAR,
    TOPIC_STATUS_MOTOR_START_ERROR,
    TOPIC_BATTERY_INFO,
    TOPIC_CONTROL_DEVICE
  };

  TopicName topicList3[] = {
    TOPIC_HARD_SYNC,
    TOPIC_GPS_SIGNAL_LEVEL,
    TOPIC_GPS_CONTROL_LEVEL,
    TOPIC_RC_FULL_RAW_DATA, //deprecated
    TOPIC_RC_WITH_FLAG_DATA,
    TOPIC_ESC_DATA,
    TOPIC_RTK_CONNECT_STATUS,
    TOPIC_GIMBAL_CONTROL_MODE,
    TOPIC_FLIGHT_ANOMALY,
    TOPIC_POSITION_VO
  };


  TopicName topicList4[] = {
    TOPIC_AVOID_DATA,
    TOPIC_HOME_POINT_SET_STATUS,
    TOPIC_HOME_POINT_INFO,
    // TOPIC_DUAL_GIMBAL_DATA,
    // TOPIC_THREE_GIMBAL_DATA,
    // TOTAL_TOPIC_NUMBER
  };


  TopicName topicList10Hz[] = {
    TOPIC_QUATERNION, TOPIC_BATTERY_INFO,
  };
	while (1)
  {
    // 0Ëß£ÈîÅ„ÄÅ1‰∏äÈîÅ„ÄÅ2Ëµ∑È£û„ÄÅ3ÈôçËêΩ„ÄÅ4ÊåáÁÇπÈ£ûË°å„ÄÅ5ËøîËà™„ÄÅ6ËÆæÁΩÆËøîËà™È´òÂ∫¶
    
    int value = sharedQueue.pop();  // Use pop method
    std::cout << "Command Received : " << value << std::endl;
    
    switch (value)
    {
      case 0:
      {
        // Ëß£ÈîÅ
        std::cout << "dis arm\n";
        vehicle->control->disArmMotors();
        break;
      }
      case 1:
      {
        // ‰∏äÈîÅ
        std::cout << "arm\n";
        vehicle->control->armMotors();
        break;
      }
      case 2:
      {
        // Ëµ∑È£û
        std::cout << "takeoff\n";
        flightSample->monitoredTakeoff();
        break;
      }
      case 3:
      {
        // ÈôçËêΩ
        std::cout << "land\n";
        flightSample->monitoredLanding();
        break;
      }
      case 4:
      {
        // ÊåáÁÇπÈ£ûË°å
        std::cout << "move:please input x,y,z,yaw offset\n";
        float nums[4];
        for (int i = 0; i < 4; i++)
        {
          std::cout << "ËØ∑ËæìÂÖ•Á¨¨ " << i + 1 << " ‰∏™Êï∞Ôºö";
          std::cin >> nums[i];
        }
        flightSample->moveByPositionOffset(
          (FlightSample::Vector3f){ nums[0], nums[1], nums[2] },
          nums[3],
          0.8,
          1);
        break;
      }
      case 5:
      {
        // ËøîËà™
        std::cout << "go home\n";
        flightSample->goHomeAndConfirmLanding();
        break;
      }
      case 6:
      {
        // ËÆæÁΩÆËøîËà™È´òÂ∫¶
        std::cout << "set go home altitude:please input altitude\n";
        float height;
        std::cin >> height;
        flightSample->setGoHomeAltitude(height);
        break;
      }


      case 11:
      {
        std::cout << "get value\n";
        int pkgIndex = 1;

        int topicSize = sizeof(topicList1) / sizeof(topicList1[0]);
        flightSample->setUpSubscription(
          pkgIndex, freq, topicList1, topicSize, timeout);

        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_QUATERNION>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_ACCELERATION_GROUND>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_ACCELERATION_BODY>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_ACCELERATION_RAW>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_VELOCITY>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_FUSIONED>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_ANGULAR_RATE_RAW>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_ALTITUDE_BAROMETER>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_ALTITUDE_OF_HOMEPOINT>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_HEIGHT_FUSION>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_FUSED>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_DATE>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_TIME>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_POSITION>());

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

        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_VELOCITY>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_GPS_DETAILS>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_POSITION>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RTK_YAW>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_RTK_POSITION_INFO>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_COMPASS>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_RC>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_GIMBAL_ANGLES>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_GIMBAL_STATUS>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_STATUS_LANDINGGEAR>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_STATUS_MOTOR_START_ERROR>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_CONTROL_DEVICE>());
        
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

        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_HARD_SYNC>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_GPS_SIGNAL_LEVEL>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_GPS_CONTROL_LEVEL>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_RC_FULL_RAW_DATA>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_RC_WITH_FLAG_DATA>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_ESC_DATA>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_RTK_CONNECT_STATUS>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_GIMBAL_CONTROL_MODE>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_FLIGHT_ANOMALY>());
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_POSITION_VO>());
        
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
        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_AVOID_DATA>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_HOME_POINT_SET_STATUS>());
        getValueFromTypeMap(
          vehicle->subscribe->getValue<TOPIC_HOME_POINT_INFO>());
        
        flightSample->teardownSubscription(pkgIndex,timeout);
        break;
      }

      case 100:
      {

        // ËÆæÁΩÆËøîËà™È´òÂ∫¶
        std::cout << "get value\n";
        int pkgIndex = 100;
        int topicSize = sizeof(topicList10Hz) / sizeof(topicList10Hz[0]);
        flightSample->setUpSubscription(
          pkgIndex, freq, topicList10Hz, topicSize, timeout);

        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_QUATERNION>());

        getValueFromTypeMap(vehicle->subscribe->getValue<TOPIC_BATTERY_INFO>());
        break;
      }
      default:
        break;
    }
  }
  
    vehicle->flightController->releaseJoystickCtrlAuthorityAsync(
    ReleaseJoystickCtrlAuthorityCB, nullptr, functionTimeout, 2);
  delete flightSample;
}

int main(int argc, char** argv)
{
  // Initialize variables

	    // ¥¥Ω® Socket Poster
	    int clientSocketPoster = socket(AF_INET, SOCK_STREAM, 0);
	    if (clientSocketPoster == -1) {
	        perror("Error creating socket");
	        return 1;
	    }
	
	    // …Ë÷√∑˛ŒÒ∆˜µƒ IP µÿ÷∑∫Õ∂Àø⁄∫≈ Poster
	    sockaddr_in serverAddrPoster;
	    serverAddrPoster.sin_family = AF_INET;
	    serverAddrPoster.sin_port = htons(5000); // ÃÊªªŒ™∑˛ŒÒ∆˜µƒ∂Àø⁄∫≈
	    serverAddrPoster.sin_addr.s_addr = inet_addr("192.168.10.83"); // ÃÊªªŒ™∑˛ŒÒ∆˜µƒ IP µÿ÷∑
	
	    // ¥¥Ω®Socket
	    int serverSocketReceiver = socket(AF_INET, SOCK_STREAM, 0);
	    if (serverSocketReceiver == -1) {
	        perror("Error creating socket");
	        return 1;
	    }
	
	    // ∞Û∂®IPµÿ÷∑∫Õ∂Àø⁄
	    sockaddr_in serverAddrReceiver;
	    serverAddrReceiver.sin_family = AF_INET;
	    serverAddrReceiver.sin_port = htons(5001); //  π”√∂Àø⁄12345
	    serverAddrReceiver.sin_addr.s_addr = INADDR_ANY; // º‡Ã˝À˘”–Õ¯ø®…œµƒ¡¨Ω”

	thread posterThread(postData, clientSocketPoster, serverAddrPoster);
    thread receiverThread(receiveData, serverSocketReceiver, serverAddrReceiver, std::ref(sharedCommandQueue));
	thread consumerThread(uavExecute, argc, argv, std::ref(sharedCommandQueue));    
	
	receiverThread.join();
    posterThread.join();
    consumerThread.join();
    
    close(clientSocketPoster);
    close(serverSocketReceiver);
    
  return 0;
}
