'''
Tencent cloud 接收天翼云(其他也可，无需更改任何参数)
'''
import socket
import sys
from threading import Thread

def socket_service_data(address):
    """
    Connect and receive data

    Arguments:
    address   -   ('cloud private mac',port)
    """
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(address)  # 在不同主机或者同一主机的不同系统下使用实际ip
        s.listen(10)
    except socket.error as msg:
        print(msg)
        sys.exit(1)
    print("..................Wait for Connection..................")

    sock, addr = s.accept()
    
    while True:
        buf = sock.recv(BUFSIZ)  # 接收数据
        buf1 = buf.decode('utf-8')  # 解码
        if not buf:
            break
        print('Received message:', buf1)
    sock.close()


if __name__ == '__main__':
    HOST = '10.0.12.13' # 腾讯云内网IP
    BUFSIZ = 1024
    ADDR2 = (HOST, 5000) # 开放5000端口

    Thread(target = socket_service_data,args=(ADDR2,)).start()