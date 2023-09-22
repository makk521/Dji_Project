#coding= utf-8
#!/usr/bin/env python3
import time
import serial
import threading
import cmath
import math
import socket
import json
import time
import re

from mavlink import *

class DeviceSerial(object):
    """
    """
    def __init__(self, dev_n, baud):
        """
        dev_n: serial device, For example, dev_n = '/dev/ttyACM0'
        baud: baudrate, For example, baud=115200
        """
        self.dev         = serial.Serial(dev_n, baud, timeout=None)

    def write(self, buf):
        send_callback = self.dev.write(buf)
    def read(self, size):
        buf = self.dev.read(size)
        return buf
    def close(self):
        try:
            self.dev.close()
        except Exception as e:
            print(e)
    
class MAVAPI(object):
    """
    """
    def __init__(self, dev_n, baud):
        # 必要的成员变量
        self.sys_id = 254
        self.cmp_id = 1
        self.tgt_system_id    = 1
        self.tgt_component_id = 1
        # 实例化串口底层通信类        
        self.dev = DeviceSerial(dev_n, baud)
        # 实例化MAVLink通信类
        self.mav = MAVLink(self.dev, self.sys_id, self.cmp_id)

        # 用于保存接收到的mavlink消息
        self.cur_mav_msg = None
        # 系统启动时间
        self.sys_time_startup = time.time()
        # 各类线程实例，并开启线程
        self.th_msg_receive    = threading.Thread(daemon=True, target=self.th_receive_fun)
        self.th_send_heartbeat = threading.Thread(daemon=True, target=self.th_send_heartbeat_fun)
        self.th_msg_receive.start()
        self.th_send_heartbeat.start()

    def th_receive_fun(self):
        """
        线程回调函数：一直接收数据并解析
        """
        getMessageNum=0
        while True:
            data = self.dev.read(8)
            if len(data)>0:
                # 收到数据，解析mavlink消息
                try:
                    self.cur_mav_msg = self.mav.parse_char(data)
                except Exception as e:
                    continue
                try:
                    if isinstance(self.cur_mav_msg, MAVLink_message):
                        getMessageNum+=1
                        # print("num=",getMessageNum)
                        # print(type(self.cur_mav_msg))
                        pass
                    if isinstance(self.cur_mav_msg, MAVLink_command_ack_message):
                        # "command", "result", "progress", "result_param2", "target_system", "target_component"
                        print(getMessageNum,"指令反馈,type=",type(self.cur_mav_msg))
                        print(self.cur_mav_msg)
                    if isinstance(self.cur_mav_msg, MAVLink_heartbeat_message):
                        # self, type, autopilot, base_mode, custom_mode, system_status, mavlink_version
                        # print(getMessageNum,"心跳包,type=",type(self.cur_mav_msg))
                        # print(self.cur_mav_msg)
                        # print("INFO: sys={}, cmp={}".format(self.cur_mav_msg.get_srcSystem(), self.cur_mav_msg.get_srcComponent()))
                        # print("INFO: type={}, autopilot={}".format(self.cur_mav_msg.type, self.cur_mav_msg.autopilot))
                        # print("INFO: base_mode={}, custom_mode={}".format(self.cur_mav_msg.base_mode, self.cur_mav_msg.custom_mode))
                        pass
                    elif isinstance(self.cur_mav_msg, MAVLink_attitude_message):
                        # "time_boot_ms", "roll", "pitch", "yaw", "rollspeed", "pitchspeed", "yawspeed"
                        # “时间引导ms”、“横滚”、“俯仰”、“偏航”、“纵滚速度”、“变桨速度”和“偏航速度”
                        # print("num=",getMessageNum,",type=",type(self.cur_mav_msg))
                        # print("INFO: roll={:.3f}, pitch={:.3f}, yaw={:.3f}".format(self.cur_mav_msg.roll, self.cur_mav_msg.pitch, self.cur_mav_msg.yaw))
                        self.roll_NED  = self.cur_mav_msg.roll
                        self.pitch_NED = self.cur_mav_msg.pitch
                        self.yaw_NED   = self.cur_mav_msg.yaw

                    elif isinstance(self.cur_mav_msg, MAVLink_gps_raw_int_message):
                        # "time_usec", "fix_type", "lat", "lon", "alt", "eph", "epv", "vel", "cog", 
                        # “时间戳” “GPS类型” “经度” “维度” “高度” “水平精度因子” “垂直精度因子” “全球定位系统地速度”  “实际航迹向”
                        # "satellites_visible", "alt_ellipsoid", "h_acc", "v_acc", "vel_acc", "hdg_acc", "yaw"
                        # “卫星可见数” “海拔” “位置不确定度” “高度不确定度” “速度不确定度” “航向/航迹不确定性” “偏转”
                        # print("num=",getMessageNum,",type=",type(self.cur_mav_msg))
                        # print("fix_type:{}, lat={:.6f}, lon={:.6f}, alt={:.3f}".format(self.cur_mav_msg.fix_type, self.cur_mav_msg.lat, self.cur_mav_msg.lon, self.cur_mav_msg.alt))
                        # t1=time.time()%10000
                        # t2=self.cur_mav_msg.time_usec/1000000
                        # print("时间戳差值：",t1,t2,t1-t2)
                        self.lat = self.cur_mav_msg.lat/10000000
                        self.lon = self.cur_mav_msg.lon/10000000
                        self.alt = self.cur_mav_msg.alt/1000

                    # elif isinstance(self.cur_mav_msg, MAVLink_extended_sys_state_message):
                    #     #vtol_state  landed_state
                    #     print(getMessageNum,"飞行状态,type=",type(self.cur_mav_msg))
                    #     print("vtol_state:{:.3f}, landed_state：{:.3f}".format(self.cur_mav_msg.vtol_state, self.cur_mav_msg.landed_state))
                    # # elif isinstance(self.cur_mav_msg, MAVLink_vfr_hud_message):
                    # #     print(getMessageNum,"通常显示的指标,type=",type(self.cur_mav_msg))
                    # #     print("airspeed:{:.2f}, groundspeed:{:.2f}, heading:{:.2f}".format(self.cur_mav_msg.airspeed, self.cur_mav_msg.groundspeed,self.cur_mav_msg.heading))
                    # #     print("throttle:{:.2f}, climb:{:.2f}".format(self.cur_mav_msg.throttle,self.cur_mav_msg.climb))
                    # elif isinstance(self.cur_mav_msg, MAVLink_mission_current_message):
                    #     print(getMessageNum,",type=",type(self.cur_mav_msg))
                    #     print("*seq:{}, total:{}, mission_state:{}, mission_mode:{}".format(self.cur_mav_msg.seq, self.cur_mav_msg.total,self.cur_mav_msg.mission_state,self.cur_mav_msg.mission_mode))
                    # elif isinstance(self.cur_mav_msg, MAVLink_nav_controller_output_message):
                    #     print(getMessageNum,"控制器状态,type=",type(self.cur_mav_msg))
                    #     print("*nav_roll:{:.2f}, nav_pitch:{:.2f}, nav_bearing:{:.2f}".format(self.cur_mav_msg.nav_roll, self.cur_mav_msg.nav_pitch,self.cur_mav_msg.nav_bearing))
                    #     print("target_bearing:{:.2f}, wp_dist:{:.2f}".format(self.cur_mav_msg.target_bearing,self.cur_mav_msg.wp_dist))
                except Exception as e:
                    print(e)
                   
    def th_send_heartbeat_fun(self):
        """
        线程回调函数：以一定频率发送心跳包
        """
        while True:
            ### send as a onboard computer or GCS
            #msg = MAVLink_heartbeat_message(type=MAV_TYPE_GCS, autopilot=MAV_AUTOPILOT_INVALID, base_mode=0, custom_mode=0, 
            #                                system_status=MAV_STATE_ACTIVE, mavlink_version=3)
            ### send as a drone simulator
            msg = MAVLink_heartbeat_message(type=MAV_TYPE_QUADROTOR, autopilot=MAV_AUTOPILOT_PX4, base_mode=MAV_MODE_PREFLIGHT, custom_mode=0x30000, 
                                            system_status=MAV_STATE_ACTIVE, mavlink_version=3)
    
            self.mav.send(msg)
            time.sleep(0.1)

    def cmd_disarm(self, arm):
        """
        发送解锁/上锁指令
        arm:
            - False: disarm
            - True: arm
        """
        arm = 1 if arm else 0
        # id="76"
        msg_cmd = MAVLink_command_long_message(self.tgt_system_id, self.tgt_component_id, MAV_CMD_COMPONENT_ARM_DISARM, 0, arm, 0, 0, 0, 0, 0, 0)
        self.mav.send(msg_cmd)
    
    def takeoff_land(self, cmd='takeoff'):
        """
        发送起飞指令，通过MAVLink SET_MODE(#11)消息实现原地起飞
        since="2015-12" replaced_by="MAV_CMD_DO_SET_MODE"
        """
        base_mode   = 129
        if cmd == 'takeoff':
            custom_mode = 0x2040000
        elif cmd == 'land':
            custom_mode = 0x6040000
        # id="11"
        msg = MAVLink_set_mode_message(self.tgt_system_id, base_mode=base_mode, custom_mode=custom_mode)
        self.mav.send(msg)
# ################
    def getSysState(self):
        """
        MAVLink_extended_sys_state_message 245 获取飞行状态
        """
        msg = MAVLink_extended_sys_state_message(vtol_state=MAV_VTOL_STATE_UNDEFINED , landed_state=MAV_LANDED_STATE_UNDEFINED )
    
        self.mav.send(msg)
        time.sleep(1)
    def getGPS(self):
        """
        GPS_RAW_INT ( #24 )
        MAVLink_gps_raw_int_message
       
        """
        msg = MAVLink_gps_raw_int_message(time_usec=0, fix_type=GPS_FIX_TYPE_3D_FIX, lat=0, lon=0, alt=0, eph=0, epv=0, vel=0, cog=0, satellites_visible=0, alt_ellipsoid=0, h_acc=0, v_acc=0, vel_acc=0, hdg_acc=0, yaw=0 )
    
        self.mav.send(msg)
        time.sleep(1)    
    
    """
    COMMAND_LONG ( #76 )
    用于发送一些列操作指令：系统id 组件id 命令id 
    target_system	uint8_t		System which should execute the command
    target_component	uint8_t		Component which should execute the command, 0 for all components
    command	uint16_t	MAV_CMD	Command ID (of command to send).
    confirmation	uint8_t		0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)
    param1	float		Parameter 1 (for the specific command).
    param2	float		Parameter 2 (for the specific command).
    param3	float		Parameter 3 (for the specific command).
    param4	float		Parameter 4 (for the specific command).
    param5	float		Parameter 5 (for the specific command).
    param6	float		Parameter 6 (for the specific command).
    param7	float		Parameter 7 (for the specific command).
    """
    def cmdTakeoff22(self):
        """
        MAV_CMD_NAV_TAKEOFF 22 起飞
        其中只有参数7有用到，参数7为海拔高度，其他参数默认给0即可.
        7: Altitude	Altitude	m
        """
        
        height=0.5
        print(self.alt+height)
        msg_cmd = MAVLink_command_long_message(self.tgt_system_id, self.tgt_component_id, MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, 0, 0, 0, self.alt+height)
        self.mav.send(msg_cmd)
    def getHeight(self):
        print("alt:",self.alt)
        return self.alt

    def cmdTakeoff24(self):
        """
        MAV_CMD_NAV_TAKEOFF_LOCAL 24 起飞
        从本地位置起飞（仅限本地帧
        3   Takeoff ascend rate
        4   Yaw angle (if magnetometer or another yaw estimation source present), ignored without one of these
        5 Y-axis position
        6 X-axis position
        7 Z-axis position
        """
        msg_cmd = MAVLink_command_long_message(self.tgt_system_id, self.tgt_component_id, MAV_CMD_NAV_TAKEOFF_LOCAL,0, 0, 0, 0, 0, 0, 0, 0.2)
        self.mav.send(msg_cmd)
    def cmdLand(self):
        """
        MAV_CMD_NAV_LAND  21 降落
        其中只有参数456有用到，其他参数默认给0即可.		
        4: Yaw Angle	Desired yaw angle. NaN to use the current system yaw heading mode (e.g. yaw towards next waypoint, yaw to home, etc.).		deg
        5: Latitude	Latitude.		
        6: Longitude	Longitude.		
        7: Altitude	Landing altitude (ground level in current frame).		m
        """

        msg_cmd = MAVLink_command_long_message(self.tgt_system_id, self.tgt_component_id, MAV_CMD_NAV_LAND, 0, 0, 0, 0, cmath.nan, self.lat,self.lon, 0)
        self.mav.send(msg_cmd)
    def cmdReturn(self):
        """
        MAV_CMD_NAV_RETURN_TO_LAUNCH  20 返航
        参数无用到	
        """

        msg_cmd = MAVLink_command_long_message(self.tgt_system_id, self.tgt_component_id, MAV_CMD_NAV_RETURN_TO_LAUNCH, 0, 0, 0, 0, 0, 0, 0, 0)
        self.mav.send(msg_cmd)
    def cmdDigicam(self):
        """
        MAV_CMD_DO_DIGICAM_CONTROL  203 相机控制
        [Command] Control digital camera. This is a fallback message for systems that have not yet implemented
        PARAM_EXT_XXX messages and camera definition files (see https://mavlink.io/en/services/camera_def.html ).
        1: Session Control, Session control e.g. show/hide lens
        2: Zoom Absolute,	Zoom's absolute position
        3: Zoom Relative,	Zooming step value to offset zoom from the current position
        4: Focus,	Focus Locking, Unlocking or Re-locking
        5: Shoot Command,	Shooting Command
        6: Command Identity,	Command Identity
        7: Shot ID,	Test shot identifier. If set to 1, image will only be captured, but not counted towards internal frame count.	
        """
        msg_cmd = MAVLink_command_long_message(self.tgt_system_id, self.tgt_component_id, MAV_CMD_DO_DIGICAM_CONTROL, 0, 0, 0, 0, 0, 1, 0, 0)
        self.mav.send(msg_cmd)
    def cmdSetMode(self,modeType):
        """
        MAV_CMD_DO_SET_MODE  176 设置飞行模式
        定点模式：point
        定高模式：height
        任务模式：task
        返航模式：return
        """
        if modeType=="point":
            msg_cmd = MAVLink_command_long_message(self.tgt_system_id,self.tgt_component_id,MAV_CMD_DO_SET_MODE,0, 1 , 3, 0.0, 0, 0.0, 0.0, 0.0)
        elif modeType=="height":
            msg_cmd = MAVLink_command_long_message(self.tgt_system_id,self.tgt_component_id,MAV_CMD_DO_SET_MODE,0, 1 , 2, 0.0, 0, 0.0, 0.0, 0.0)
        # elif modeType=="task":
        #     msg_cmd = MAVLink_command_long_message(self.tgt_system_id,self.tgt_component_id,MAV_CMD_DO_SET_MODE,0, 1 , 0x4000000, 0.0, 0, 0.0, 0.0, 0.0)
        # elif modeType=="return":
        #     msg_cmd = MAVLink_command_long_message(self.tgt_system_id,self.tgt_component_id,MAV_CMD_DO_SET_MODE,0, 1 , 0x5000000, 0.0, 0, 0.0, 0.0, 0.0)
        self.mav.send(msg_cmd)
    def to_xy_3(M_lat, M_lon, M_alt, O_lat, O_lon, O_alt):
        # 经纬度解算
        Ea = 6378137   # 赤道半径
        Eb = 6356725   # 极半径
        M_lat = math.radians(M_lat)
        M_lon = math.radians(M_lon)
        O_lat = math.radians(O_lat)
        O_lon = math.radians(O_lon)
        Ec = Ea*(1-(Ea-Eb)/Ea*((math.sin(M_lat))**2)) + M_alt
        Ed = Ec * math.cos(M_lat)
        d_lat = M_lat - O_lat
        d_lon = M_lon - O_lon
        x = d_lat * Ec
        y = d_lon * Ed
        z = M_alt - O_alt
        return x, y, z
    def from_xy_3(self, O_lat, O_lon, O_alt,x, y, z):
        # 经纬度解算
        Ea = 6378137   # 赤道半径
        Eb = 6356725   # 极半径
        O_lat = math.radians(O_lat)
        O_lon = math.radians(O_lon)
        Ec = Ea*(1-(Ea-Eb)/Ea*((math.sin(O_lat))**2)) + O_lat
        Ed = Ec * math.cos(O_lat)
        M_lat = math.degrees(O_lat + x / Ec)
        M_lon = math.degrees(O_lon + y / Ed)
        M_alt = O_alt + z
        return M_lat, M_lon, M_alt
    def cmdReposition(self,x,y,z):
        """
        MAV_CMD_DO_REPOSITION  192 指点
        1: Speed	Ground speed, less than 0 (-1) for default	min: -1	m/s
        2: Bitmask	Bitmask of option flags.	MAV_DO_REPOSITION_FLAGS	 https://mavlink.io/en/messages/common.html#MAV_DO_REPOSITION_FLAGS
        3: Radius	Loiter radius for planes. Positive values only, direction is controlled by Yaw value. A value of zero or NaN is ignored.		m
        4: Yaw	Yaw heading. NaN to use the current system yaw heading mode (e.g. yaw towards next waypoint, yaw to home, etc.). For planes indicates loiter direction (0: clockwise, 1: counter clockwise)		deg
        5: Latitude	Latitude		
        6: Longitude	Longitude		
        7: Altitude	Altitude		m
        """
        M_lat, M_lon, M_alt=self.from_xy_3(self.lat,self.lon,self.alt,x,y,z)
        print("lat={:.6f}, lon={:.6f}, alt={:.3f}".format(M_lat,M_lon,M_alt))
        msg_cmd = MAVLink_command_long_message(1,self.tgt_component_id,MAV_CMD_DO_REPOSITION,0, 1 , ESTIMATOR_ATTITUDE, 0, 0, M_lat, M_lon, M_alt)
        self.mav.send(msg_cmd)
##################
    def exit(self):
        """
        退出，关闭UDP通信
        """
        self.dev.close()

def assignCmd(recvmsg,mavapi):
    # cmd=recvmsg['cmdName']
    cmd=recvmsg
    returnMsg=''
    if cmd=='disarm':
        # mavapi.cmdSetMode('height')
        # time.sleep(1)
        mavapi.cmd_disarm(True)
    # 定点模式：point
    # 定高模式：height
    # 任务模式：task
    # 返航模式：return
    elif cmd=='modep':
        mavapi.cmdSetMode('point')
    elif cmd=='modeh':
        mavapi.cmdSetMode('height')
    # elif cmd=='modet':
    #     mavapi.cmdSetMode('task')
    # elif cmd=='moder':
    #     mavapi.cmdSetMode('return')
    # elif cmd=='arm':
        mavapi.cmd_disarm(False)
    elif cmd=='height':
        mavapi.height() 
    elif cmd=='takeoff':
        mavapi.cmdTakeoff22()       #22指令可以起飞
    # elif cmd=='takeoff24':          #24指令无法生效
    #     mavapi.cmdTakeoff24()
    elif cmd=='land':
        mavapi.cmdLand()
    elif cmd=='return':
        mavapi.cmdReturn()
    elif cmd=='gox':
        mavapi.cmdReposition(1,0,0)
    elif cmd=='goy':
            mavapi.cmdReposition(0,-1,0)
    elif cmd=='goz':
        mavapi.cmdReposition(0,0,1)
    elif cmd=='down':
        mavapi.cmdReposition(0,0,-0.5)
    elif cmd=='geth':
        returnMsg=' height='+str(mavapi.getHeight())
    elif 'goto' in cmd:
        #num=int(cmd[3:])
        mavapi.cmdReposition(0,0,0)
    elif cmd=='finish':
        mavapi.cmd_disarm(False)
        print("over")
    else:
        print("error command!") 
        return "error command!"
        
    print("successful!"+returnMsg) 
    return "successful!"+returnMsg
def test_px4_sim():
    
    dev_n = 'COM1'      #串口名称，windows和linux有区别
    baud  = 115200
    mavapi = MAVAPI(dev_n, baud)
    
    time.sleep(1)
    
    # 创建一个tcp/ip协议的套接字
    tcpSocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

    # 创建一个udp/ip的套接字
    # udpSocket=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

    #IP/端口号
    # host = "192.168.10.151"
    host = "127.0.0.1"
    port = 8888
    Address = (host,port)
    # 开启服务端，等待客户端连接
    # 开始绑定地址
    tcpSocket.bind(Address)
    tcpSocket.listen(5)

    while 1:
        print("-"*15,"服务器等待被连接","-"*15)
        client_sock,client_add = tcpSocket.accept()
        print("已连接到客户端，地址为：{0}".format(client_add))
        try:
            print("*"*20,"正在等待接收数据ing","*"*20)
            while True:
                #接收客户端向服务器发送的数据
                Client_databack = client_sock.recv(1024)
                if not Client_databack:
                    continue;
                
                # recvmsg={
                #     'cmdName':'.',
                #     'cmdId':'',
                #     'cmdParams':[],
                # }
                try:
                    msg=Client_databack.decode('utf-8')
                    # 去掉换行等符号，只保留字母与数字
                    recvmsg=re.sub(r'[^a-zA-Z0-9]', '', str(msg))
                    print("get message",recvmsg)
                    Server_return=assignCmd(recvmsg,mavapi)
                    # recvmsg=json.loads(msg)
                    # print("get message",recvmsg['cmdName'])
                except Exception as abc:
                    print(abc)
                    print('Format error')
                    Server_data='Format error!'
                    client_sock.send(Server_data.encode("UTF-8"))
                    continue
                
                client_sock.send(Server_return.encode("UTF-8"))

        except Exception as abc:
            print(abc)

        finally:
            client_sock.close()

    
if __name__ == '__main__':
    # test_gcs()
    test_px4_sim()
