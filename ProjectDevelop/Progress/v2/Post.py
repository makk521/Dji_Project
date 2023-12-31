import requests
from pydantic import BaseModel
from typing import Optional
import json

class positionParams(BaseModel):
    x: Optional[float] = 0
    y: Optional[float] = 0
    z: Optional[float] = 0
    yaw: Optional[float] = 0

dataSend = {
    "data": {
        "uid": "2", #无人机序号
        "uavType":"", #无人机类型 大疆=dji
        "action": "takeoff", #arm disarm takeoff land move gohome setGohomeHeight
        "params": [], #takeoff:x,y,z,yaw(float xyz轴位移，无人机机头朝向) setGohomeHeight:height
        "commandNum":"", #唯一命令编号
        "timeStamp": 46523164613 , #时间戳
        "priority": '1', #1正常排队执行，2是插队优先执行（下一个执行），3是中断（停止当前指令立即执行）
        "reserve":"", #保留位
    }
}

if __name__ == "__main__":
    print(type(dataSend))
    response = requests.post('http://124.223.76.58:8000/foo', json=dataSend)
    print(response.content)
