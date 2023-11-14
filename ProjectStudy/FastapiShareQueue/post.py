import requests
from typing import Optional
import json

dataSend = {
    "code": 1,
    "data": {
        "commandNum": "2",
        "code": 10086,
        "timestamp": 432123234566612
    },
    "message": "命令执行成功"
}

if __name__ == "__main__":
    print(type(dataSend))
    response = requests.post('http://172.21.2.7:8000/foo', json=dataSend)
    print(response.content)
