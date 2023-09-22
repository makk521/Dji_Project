from fastapi import FastAPI
from typing import List
import uvicorn
from pydantic import BaseModel

class positionParams(BaseModel):
    x: float
    y: float
    z: float
    yaw: float

app = FastAPI()

@app.get("/")
def read_root():
    return {"Hello": "World"}

@app.get("/arm")
def get_item():
    """上锁"""
    return {
        'code':200,
        'message':'success(说明指令是否下达成功)',
        'data':{
            'id':'123465(无人机唯一标识码)',
            'ip':'127.0.0.1(无人机IP)',
            'message':"success(说明指令是否执行成功)",
            'returnMsg':"returnMsg(预留无人机返回信息)", 
        }
    }
@app.get("/disarm")
def get_item():
    """解锁"""
    return {
        'code':200,
        'message':'success(说明指令是否下达成功)',
        'data':{
            'id':'123465(无人机唯一标识码)',
            'ip':'127.0.0.1(无人机IP)',
            'message':"success(说明指令是否执行成功)",
            'returnMsg':"returnMsg(预留无人机返回信息)", 
        }
    }

@app.get("/takeoff/{height}")
def read_item(height: float=1.2):
    """起飞 大疆无人机自动起飞1.2 米（该高度不可调整，需要调整可在起飞之后下达一个指点飞行指令）"""
    return {
        'code':200,
        'message':'success(说明指令是否下达成功)',
        'data':{
            'id':'123465(无人机唯一标识码)',
            'ip':'127.0.0.1(无人机IP)',
            'message':"success(说明指令是否执行成功)",
            'returnMsg':"returnMsg(预留无人机返回信息)", 
        }
    }

@app.get("/land")
def get_item():
    """降落"""
    return {
        'code':200,
        'message':'success(说明指令是否下达成功)',
        'data':{
            'id':'123465(无人机唯一标识码)',
            'ip':'127.0.0.1(无人机IP)',
            'message':"success(说明指令是否执行成功)",
            'returnMsg':"returnMsg(预留无人机返回信息)", 
        }
    }

@app.post("/moveByPositionOffset/")
def create_item(position:positionParams):
    """指点飞行(以起点为原点xyz相对坐标)"""
    return {
        'code':200,
        'message':'success(说明指令是否下达成功)',
        'data':{
            'id':'123465(无人机唯一标识码)',
            'ip':'127.0.0.1(无人机IP)',
            'message':"success(说明指令是否执行成功)",
            'returnMsg':"returnMsg(预留无人机返回信息)", 
        }
    }

@app.get("/setGoHomeAltitude/{altitude}")
def read_item(height: float):
    """设置无人机返航高度"""
    return {
        'code':200,
        'message':'success(说明指令是否下达成功)',
        'data':{
            'id':'123465(无人机唯一标识码)',
            'ip':'127.0.0.1(无人机IP)',
            'message':"success(说明指令是否执行成功)",
            'returnMsg':"returnMsg(预留无人机返回信息)", 
        }
    }

@app.get("/goHome")
def get_item():
    """返航"""
    return {
        'code':200,
        'message':'success(说明指令是否下达成功)',
        'data':{
            'id':'123465(无人机唯一标识码)',
            'ip':'127.0.0.1(无人机IP)',
            'message':"success(说明指令是否执行成功)",
            'returnMsg':"returnMsg(预留无人机返回信息)", 
        }
    }

if __name__ == "__main__":

    uvicorn.run(app, host="localhost", port=8088)