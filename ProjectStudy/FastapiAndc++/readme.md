## 目标

终端调用 ``fastapi``传输，妙算通过运行脚本使用 ``fastapi``接收数据，处理后调用对应的c++函数。

## 基础

### 参考链接

[官网链接](https://fastapi.tiangolo.com/zh/)

## 实验

### 本地

```shell
pip install fastapi   
pip install "uvicorn[standard]"
```

环境配置好后运行代码：

```python
from fastapi import FastAPI

app = FastAPI()


@app.get("/")
async def root():
    return {"message": "Hello World"}

```

运行(python虚拟环境中直接运行)：

```shell
uvicorn main:app --reload
```

### uvicorn简介

`uvicorn main:app` 命令含义如下:

* `main`：`main.py` 文件（一个 Python「模块」）。
* `app`：在 `main.py` 文件中通过 `app = FastAPI()` 创建的对象。
* `--reload`：让服务器在更新代码后重新启动。仅在开发时使用该选项。

### 云端

安装一样，脚本一样，运行指令需要指定

```shell
uvicorn main:app --host 0.0.0.0 --port 5000 --reload
```

5000端口别忘了在防火墙中打开

直接访问公网Ip:

```shell
http://124.223.76.58:5000/
```

### 获取云端数据

使用 ``requests``库，``get_data_from_server``函数调用即可。

## python调用c++函数

### ctypes方法

ctypes 是 Python 的标准库模块，它允许你调用动态链接库（DLL）中的 C 函数。你可以将 C++ 函数编译成共享库，然后使用 ctypes 在 Python 中加载和调用它。

cmd终端中找到文件并设置成共享：

```shell
g++ -shared -o example.so -fPIC example.cpp

```

python端调用(ctypes无需安装):

```
from ctypes import CDLL

# 加载共享库
lib = CDLL("./example.so")

# 调用 C++ 函数
lib.hello()

```

### 测试流程

打开云端运行(虚拟环境下):

```shell
uvicorn main:app --host 0.0.0.0 --port 5000 --reload
```

本地运行 ``Receive.py``函数，虚拟环境为 ``C:\Users\ASUS\Documents\GitHub\Dji_Project\ProjectStudy\Enviroment\ProjectTest``
