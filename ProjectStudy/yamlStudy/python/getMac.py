"""
# @file getMac.py
# @author Makaka
# @date 2024-01-26
# @brief 将本机信息存入yaml文件后，通过post方法将无人机信息上传到服务器
"""
import re
import subprocess
import yaml
import json
import requests
import time

YAMLFILEDIR = '../build/bin/uav_data.yaml'
URL = "http://192.168.10.230:8083/mock/701/ucs/uav/activate"
URL = "http://192.168.20.122:8080/ucs/uav/activate"
uav_data = {
    "uavId": "1", 
    "uavMac": "",  # 待更新
    "uavTimestamp": 0,  # 待更新
    "uavName": "M300_1", 
    "uavType": "MATRICE 300",
    "uavManufacturer": "dji",
    "uavColor": "red",
    "uavSize": "810*670*430mm", 
    "uavBatteryCapacity": "5935mAh",
    "uavIp": "", # 待更新
    "uavWingType":"2"
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

def get_timestamp():
    ##############################
    # @description 获取无人机13位时间戳
    # @param Nome
    # @return timestamp_milliseconds(int)
    ##############################n
    timestamp_seconds = time.time()
    # 将时间戳转换为毫秒级别的13位时间戳
    timestamp_milliseconds = int(timestamp_seconds * 1000)
    return timestamp_milliseconds

def get_first_ip():
    ##############################
    # @description 获取无人机ip  hostname -I
    # @param Nome
    # @return ip_addresses[0] 
    ##############################n
    try:
        # 执行命令并捕获输出
        result = subprocess.run(['hostname', '-I'], capture_output=True, text=True, check=True)
        
        # 获取命令输出并以空格为分隔符拆分IP地址
        ip_addresses = result.stdout.strip().split()

        # 返回第一个IP地址
        if ip_addresses:
            return ip_addresses[0]
        else:
            return None
    except subprocess.CalledProcessError as e:
        print("Error: %s" %e)
        return None

if __name__ == "__main__":
    uav_data["uavMac"] = get_mac_address()
    uav_data["uavTimestamp"] = get_timestamp()
    uav_data["uavIp"] = get_first_ip()
    # 将字典数据存储到yaml文件中
    with open(YAMLFILEDIR, 'w') as file:
        yaml.dump(uav_data, file, default_flow_style=False)
        
    uav_json = json.dumps(uav_data)

    print(uav_json)
    # response = requests.post(URL, data=uav_json, headers={'Content-Type': 'application/json'})
    # print(response.text)

