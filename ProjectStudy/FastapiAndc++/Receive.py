import requests
from ctypes import CDLL

class DjiTools:
    """
    参数:无
    功能:请求指定网络地址数据
    """
    def get_data_from_server(self):
        server_ip = "124.223.76.58:5000"  # 替换为你的云服务器的实际 IP 地址
        server_url = f"http://{server_ip}"

        # 发送 GET 请求到云服务器
        response = requests.get(f"{server_url}/")  # 替换为你的 GET 路由路径

        if response.status_code == 200:
            return {"data_from_server": response.json()}
        else:
            return {"error": "Failed to fetch data from server"}

    """
    参数:无
    功能:调用对应c++函数测试版
    """
    def call_c_function(self):
        # 加载共享库
        lib = CDLL("./example.so")
        # 调用 C++ 函数
        lib.hello()
        lib.world()

if __name__ == "__main__":
    myDji = DjiTools()
    myDji.call_c_function()
    
    # 获取云端数据并打印结果
    result = myDji.get_data_from_server()
    print(result)

