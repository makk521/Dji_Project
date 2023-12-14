"""
# @file postInfoOnline.py
# @author Makaka
# @date 2023-12-12
# @brief 通过post方法将无人机信息上传到服务器
"""
import re
import subprocess
import yaml
import json
import requests

URL = "http://192.168.10.230:8083/mock/701/ucs/uav/activate"
URL = "http://192.168.20.122:8080/ucs/uav/activate" # 测试🔗
uav_data = {
    "uavId": "1",
    "uavMac": "",
    "uavTimestamp": 0,
    "uavName": "M300_1",
    "uavType": "MATRICE 300",
    "uavWingType": 1, # 机翼类型：1-固定翼，2-螺旋翼
    "uavManufacturer": "dji",
    "uavColor": "red",
    "uavSize": "810*670*430mm",
    "uavBatteryCapacity": "5935mAh",
    "uavIp": "",
    "uavPort": ""
}

def get_mac_address(interface="eth0"):
    ##############################
    # @description 获取无人机MAC地址
    # @param interface(实际网卡名称)
    # @return mac_address(string)
    ##############################n
    try:
        # 执行系统命令获取ifconfig输出
        result = subprocess.check_output(["ifconfig", interface])
        result = result.decode("utf-8")  # 将字节转换为字符串（仅适用于Python 3）

        # 使用正则表达式从ifconfig输出中提取MAC地址
        mac_pattern = re.compile(r"HWaddr\s+([0-9a-fA-F:]+)")
        match = mac_pattern.search(result)

        if match:
            mac_address = match.group(1)
            return mac_address
        else:
            print("无法找到MAC地址。")
            return None

    except subprocess.CalledProcessError:
        print("执行ifconfig命令时出现错误。")
        return None


if __name__ == "__main__":
    uav_data["uavMac"] = get_mac_address()
    uav_json = json.dumps(uav_data)

    print(uav_json)
    response = requests.post(URL, data=uav_json, headers={'Content-Type': 'application/json'})
    print(response.text)

