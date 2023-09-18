"""
教程,Fastapi的路由函数与普通函数之间共享队列
"""
import logging
import time
from fastapi import FastAPI
from fastapi_utils.tasks import repeat_every
import uvicorn
import queue

logger = logging.getLogger(__name__)
app = FastAPI()
counter = 0
q = queue.Queue()

@app.get('/')
def hello():
    q.put(1)
    print(q, q.qsize())
    return 'Hello'

@app.on_event("startup")
@repeat_every(seconds=1, logger=logger, wait_first=True)
def periodic():
    while True:
        print(q,q.qsize())
        time.sleep(3)

if __name__ == "__main__":
    uvicorn.run("Constan_exe:app", host="0.0.0.0", port=8000, workers=1)
