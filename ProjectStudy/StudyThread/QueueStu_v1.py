"""
验证空queue进行取出命令时是监听的状态
定义两个线程,一个将数据存进去并延迟,另一个线程执行取的操作,一旦存进去则能够立刻取出来,空queue在取数据时处于监听状态
"""
from collections.abc import Callable, Iterable, Mapping
import threading
import queue
from typing import Any
import time
import sys

class myThread(threading.Thread):
    def __init__(self, threadName, q):
        threading.Thread.__init__(self)
        self.threadName = threadName
        self.q = q
    
    def run(self):
        while True:
            self.q.put(input("添加数据进入queue : "))
            time.sleep(2)

def getQueue(q):
    while True:
        print(f"Get one data from queue : {q.get()}")
        time.sleep(3)

if __name__ == "__main__":
    q = queue.Queue(2)

    thread1 = myThread("thread1", q)
    thread2 = threading.Thread(target=getQueue, args=(q, ))
    thread2.daemon = True  # 将thread1设置为后台线程
    try:
        thread1.start()
        thread2.start()
        thread1.join()
    except KeyboardInterrupt:
        print("\nCtrl+C 执行退出程序操作.")
        # 可以在这里进行清理工作
        sys.exit(0)
