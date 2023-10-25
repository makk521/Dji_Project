"""
7号机
Fastapi定义网络接口,并通过此接受来自客户端的数据
获得数据后存入队列,并通过socket取出该数据传给c++
"""
from fastapi import FastAPI
from pydantic import BaseModel
from typing import Optional
from fastapi_utils.tasks import repeat_every
import uvicorn
import threading
from threading import Thread
import time
import socket
import sys
import queue
import logging

MAXQUEUESIZE = 20
ADDR = ('192.168.10.83', 5001) # 发送目标IP

app = FastAPI()
shareQueue = queue.Queue(maxsize=MAXQUEUESIZE)
queueLock = threading.Lock()
logger = logging.getLogger(__name__)
startTime = 1
endTime  = 1

class positionParams(BaseModel):
    x: Optional[float] = 0
    y: Optional[float] = 0
    z: Optional[float] = 0
    yaw: Optional[float] = 0

@app.get('/') 
def helloWorld():
    return "hello world"

@app.post('/foo/')
def getName(info: dict):
    """
    将请求来的数据存入队列,info为查询参数,无需在路径中定义直接传过来解析即可
    """
    global shareQueue,startTime
    startTime = time.time()
    queueLock.acquire()
    print(type(info))
    try:
        shareQueue.put(int(info["data"]["commandNum"])) 
    except Exception as ex:
        print(ex)
        print("格式错误")
    queueLock.release()
    print(info)

    return {
            "code": 200, #反馈是否能接收到命令
            "message":"success"
        }


"""
放在该修饰器下可以解决Fastapi的路由函数与普通函数全局队列不共享的问题
"""
@app.on_event("startup")
@repeat_every(seconds=1, logger=logger, wait_first=True)
def periodic():
    """
    连接socket,设定get延时,若队列中存在数据则传给c++,没有则跳过继续等
    """
    global shareQueue, queueLock,endTime
    try:
        print("Start connect!")
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(ADDR)
    except socket.error as msg:
        print(msg)
        print(sys.exit(1))
    print("Connect Sucess!")
    while True:
        print("开始运行")
        try:
            print(shareQueue,shareQueue.qsize())
            sendData = str(shareQueue.get(3))
            s.send(sendData.encode('utf-8'))  # 将要传输的数据编码发送，如果是字符数据就必须要编码发送
            endTime = time.time()
            print(f"用时：{endTime - startTime}")
        except queue.Empty:
            print('队列为空,get失败')
        except Exception as ex:
            print("出现如下异常%s"%ex)
            time.sleep(1)

def receiveData(address):
    """
    Connect and receive data

    Arguments:
    address   -   ('cloud private mac',port)
    """
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(address)  # 在不同主机或者同一主机的不同系统下使用实际ip
        s.listen(10)
    except socket.error as msg:
        print(msg)
        sys.exit(1)
    print("..................Wait for Connection..................")

    sock, addr = s.accept()
    
    while True:
        buf = sock.recv(1024)  # 接收数据
        buf1 = buf.decode('utf-8')  # 解码
        if not buf:
            break
        print('Received message:', buf1)
    sock.close()

if __name__ == "__main__":
    HOST = '172.17.0.1' # 本机私有ip
    ADDR2 = (HOST, 5000)   # receiveData
    Thread(target = receiveData,args=(ADDR2,)).start()
    uvicorn.run("CloudThread:app", host="0.0.0.0", port=8000, workers=1)
