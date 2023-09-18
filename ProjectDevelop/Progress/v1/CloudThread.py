"""
Fastapi定义网络接口,并通过此接受来自客户端的数据
获得数据后存入队列,并通过socket取出该数据传给c++
"""
from fastapi import FastAPI
from pydantic import BaseModel
from typing import Optional
import uvicorn
import threading
from threading import Thread
import time
import socket
import sys
import queue
from fastapi_utils.tasks import repeat_every
import logging

MAXQUEUESIZE = 20
ADDR = ('124.223.76.58', 5000)

app = FastAPI()
shareQueue = queue.Queue(maxsize=MAXQUEUESIZE)
queueLock = threading.Lock()
logger = logging.getLogger(__name__)
start_time = 1
end_time  = 1

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

@app.post('/foo/{name}')
def getName(name, info: Student):
    """
    将请求来的数据存入队列,info为查询参数,无需在路径中定义直接传过来解析即可
    """
    global shareQueue,start_time
    start_time = time.time()
    queueLock.acquire()
    shareQueue.put(info.age)
    queueLock.release()
    print({'name:': name, 'age': info.age, 'is_man': info.is_man})
    return {'name:': name, 'age': info.age, 'is_man': info.is_man}

"""
放在该修饰器下可以解决Fastapi的路由函数与普通函数全局队列不共享的问题
"""
@app.on_event("startup")
@repeat_every(seconds=1, logger=logger, wait_first=True)
def periodic():
    """
    连接socket,设定get延时,若队列中存在数据则传给c++,没有则跳过继续等
    """
    global shareQueue, queueLock,end_time
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
            end_time = time.time()
            print(f"用时：{end_time - start_time}")
        except queue.Empty:
            print('队列为空,get失败')
        except Exception as ex:
            print("出现如下异常%s"%ex)
            time.sleep(1)

if __name__ == "__main__":
    uvicorn.run("CloudThread:app", host="0.0.0.0", port=8000, workers=1)
