# 合并request与socket通信，请求云端数据并通过socket传给c++
import socket
import requests
from ctypes import CDLL

class DjiTools:
    """
    参数:无
    功能:请求指定网络地址数据
    """
    def getDataFromServer(self):
        # server_ip = "124.223.76.58:5000"  # 替换为你的云服务器的实际 IP 地址
        # server_url = f"http://{server_ip}"
        server_url = "http://124.223.76.58:5000/"
        # 发送 GET 请求到云服务器
        response = requests.get(server_url)  # 替换为你的 GET 路由路径

        if response.status_code == 200:
            return {"data_from_server": response.json()}
        else:
            return {"error": "Failed to fetch data from server"}

    """
    参数:无
    功能:调用对应c++函数测试版
    """
    def callCFunction(self):
        # 加载共享库
        lib = CDLL("./example.so")
        # 调用 C++ 函数
        lib.hello()
        lib.world()

    """
    参数:发送数据(Str)
    功能:连接socket并发送信息
    """
    def postMessageSocket(self, sendData):
        # 服务器地址和端口
        server_address = ('localhost', 8888)  # 此处应与C++服务器的地址和端口匹配
        # 创建Socket客户端
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # 连接到服务器
        client_socket.connect(server_address)
        # 发送数据到服务器
        message = str(sendData)
        client_socket.send(message.encode())
        # 关闭Socket连接
        client_socket.close()

if __name__ == "__main__":
    myDji = DjiTools()
    # 获取云端数据并打印结果
    result = myDji.getDataFromServer()
    myDji.postMessageSocket(result)
    print(result)


