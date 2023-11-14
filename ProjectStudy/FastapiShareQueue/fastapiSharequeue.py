"""
# @file fastapiSharequeue.py
# @author Makaka
# @date 2023-11-14
# @brief 当数据通过fastapi被程序获取后并将其存入队列,如何在另一个线程操作该队列(对比两种方法,只有一种成功)
#        改代码直接运行在USTC_7,fastapi==0.103.1
"""
from fastapi import FastAPI
from fastapi_utils.tasks import repeat_every
import uvicorn
import queue
import threading
from threading import Thread
import logging

MAXQUEUESIZE = 20
app = FastAPI()
shareQueue = queue.Queue(maxsize=MAXQUEUESIZE)
queueLock = threading.Lock()
logger = logging.getLogger(__name__)

@app.post('/foo/')
def getName(info: dict):
    ##############################
    # @description 将请求来的数据存入队列,info为查询参数,无需在路径中定义直接传过来解析即可
    # @param 
    # @return 
    ##############################
    global shareQueue
    queueLock.acquire()
    try:
        shareQueue.put(info)
    except Exception as ex:
        print("格式错误" + str(ex))
    queueLock.release()
    print("已将数据存入队列：" + str(info))
    return {
            "code": 200, #反馈是否能接收到命令
            "message":"success"
        }

@app.on_event("startup")
@repeat_every(seconds=1, logger=logger, wait_first=True)
def periodic():
    ##############################
    # @description 试图取出shareQueue数据
    # @param seconds  每隔1秒执行一次
    # @return None
    ##############################
    global shareQueue, queueLock,endTime
    while True:
        print("开始运行")
        try:
            print(shareQueue,shareQueue.qsize())
            print("测试是否阻塞")
            sendData = str(shareQueue.get(3))   # 阻塞的
            print("取出队列：" + sendData)
        except queue.Empty: 
            print('队列为空,get失败')
        except Exception as ex:
            print("出现如下异常%s"%ex)

def consumeQueue():
    ##############################
    # @description 用普通方法尝试操作shareQueue
    # @param None
    # @return None
    ##############################
    global shareQueue
    while True:
        print("开始运行")
        try:
            print(shareQueue,shareQueue.qsize())
            print("测试是否阻塞")
            print("取出队列：" + str(shareQueue.get(3)))
        except Exception as ex:
            print(f"出现异常：{ex}")

if __name__ == "__main__":
    # Thread(target = consumeQueue).start()
    uvicorn.run(app="fastapiSharequeue:app", host="0.0.0.0", port=8000, workers=1)
