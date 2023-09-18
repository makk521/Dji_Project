"""
验证Queue与锁
定义两个线程与一个大小为10的queue,一个线程填入5个1,另一个线程填入2,保证得到的queue不乱
"""
from collections.abc import Callable, Iterable, Mapping
import queue
import threading
from typing import Any 

class myThread(threading.Thread):
    def __init__(self, q, threadName, count, targetNum):
        threading.Thread.__init__(self)
        self.q = q
        self.name = threadName
        self.count = count
        self.targetNum = targetNum
       
    def run(self):
        """
        put count target number into queue
        """
        print(f"{self.name} starting!")
        self.exeQueue()
        print(f"{self.name} finish!")
        print(f"线程id为{threading.currentThread().ident},线程名为{threading.currentThread().getName()}")

    def exeQueue(self):
        qLock.acquire()
        for i in range(self.count):
            q.put(self.targetNum)
        qLock.release()

if __name__ == "__main__":
    qLock = threading.Lock()
    q = queue.Queue(10)
    print(f"定义新的Queue : {q},线程id为{threading.currentThread().ident},线程名为{threading.currentThread().getName()}")
    thread1 = myThread(q, "change5", 5, 1)
    thread2 = myThread(q, "change6", 5, 2)

    thread1.start()
    thread2.start()

    thread1.join()
    thread2.join()

    for i in range(10):
        print(q.get())
