import requests
from typing import Optional
import json

dataSend = {
    "lineFormat": 1,  
    "leader": "1",  
    "data": [
        {
            "uid": "1",
            "action": "move", 
            "sParams": {"x":0, "y":0, "z":1.3}, 
            "dParams": {"x": 3, "y": 3, "z": 3},
            "commandNum": "cmdNum:003:move:1704444270941",
            "timeStamp": "1704444270941s",
            "priority": "1",
        }
    ]

}

if __name__ == "__main__":
    print(type(dataSend))
    response = requests.post('http://192.168.10.83:8000/foo', json=dataSend)
    print(response.content)
