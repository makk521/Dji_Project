from fastapi import FastAPI
from pydantic import BaseModel
from typing import Optional
import uvicorn
import threading
from threading import Thread
import time
import socket
import sys

app = FastAPI()

class Student(BaseModel):
    name: str
    age: int
    is_man: Optional[bool] = None

@app.get('/')
def helloWorld():
    return "hello world"

@app.get('/foo/{name}')
def getName(name, age: Optional[int] = None):
    print({'name:': name, 'age': age})
    return {'name:': name, 'age': age}

@app.put('/foo/{name}')
def getName(name, info: Student):
    print(type(info))
    print({'name:': name, 'age': info.age, 'is_man': info.is_man})
    return {'name:': name, 'age': info.age, 'is_man': info.is_man}

@app.post('/foo/{name}')
def getName(name, info: Student):
    print(type(info))
    print({'name:': name, 'age': info.age, 'is_man': info.is_man})
    return {'name:': name, 'age': info.age, 'is_man': info.is_man}

def printIfo(mesg):
    while(1):
        print(mesg)
        time.sleep(3)

def sock_client_data(address,send_data):
    """
    Connect to cloud and send data(every three seconds)

    Arguments:
    address    - ('cloud public mac', port)
    send_data  
    """
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(address)
    except socket.error as msg:
        print(msg)
        print(sys.exit(1))

    while True:
        s.send(send_data.encode('utf-8'))  # 将要传输的数据编码发送，如果是字符数据就必须要编码发送
        time.sleep(3)

if __name__ == "__main__":
    # 作为客户端不断发送数据给服务端(c++)
    ADDR2 = ('124.223.76.58', 5000)
    Thread(target = sock_client_data,args=(ADDR2,"a")).start()

    Thread(target = printIfo,args=("ka",)).start()
    # 一直开启api接口
    Thread(uvicorn.run('ReceiveDataCloudThread:app', host='0.0.0.0', port=5000, reload=True)).start # 阻塞的
    
