# 调试用
import socket

# 服务器地址和端口
server_address = ('localhost', 8888)  # 此处应与C++服务器的地址和端口匹配

# 创建Socket客户端
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 连接到服务器
client_socket.connect(server_address)

# 发送数据到服务器
message = "Hello from Python!"
client_socket.send(message.encode())

# 关闭Socket连接
client_socket.close()
