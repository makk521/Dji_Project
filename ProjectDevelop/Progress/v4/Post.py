import requests
from typing import Optional
import json

dataSend = {

    "uid": "2", #无人机序号
    "uavType":"", #无人机类型 大疆=dji
    "action": "setGohomeHeight", #arm disarm takeoff land move gohome setGohomeHeight
    "params": {"x":1, "y":1, "z":2, "yaw":1, "height":21}, #takeoff:x,y,z,yaw(float xyz轴位移，无人机机头朝向) setGohomeHeight:height    
    "commandNum":"11", #唯一命令编号
    "timeStamp": "46523164613" , #时间戳
    "priority": '1', #1正常排队执行，2是插队优先执行（下一个执行），3是中断（停止当前指令立即执行）
    "reserve":"", #保留位

    # "code": 1,
    # "data": {
    #     "commandNum": "2",
    #     "code": 10086,
    #     "timestamp": 432123234566612
    # },
    # "message": "命令执行成功"
}

if __name__ == "__main__":
    print(type(dataSend))
    response = requests.post('http://192.168.10.83:8000/foo', json=dataSend)
    print(response.content)
