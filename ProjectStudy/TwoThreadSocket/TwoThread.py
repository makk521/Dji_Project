'''
先运行,ustc
'''
import socket
import sys
import time
from threading import Thread

def postData(address, send_data):
    """
    Connect to cloud and send data(every three seconds)

    Arguments:
    address    - ('cloud public mac', port)
    send_data  
    """
    print("Post delay 10 seconds!")
    time.sleep(10)
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(address)
    except socket.error as msg:
        print(msg)
        print(sys.exit(1))

    while True:
        s.send(send_data.encode('utf-8'))  # 将要传输的数据编码发送，如果是字符数据就必须要编码发送
        time.sleep(3)

def receiveData(address):
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
    ADDR1 = ('124.223.76.58', 5000)  #  postData
    HOST = '192.168.1.212' # 本机私有ip
    BUFSIZ = 1024
    ADDR2 = (HOST, 5000)   # receiveData
    
    Thread(target = postData,args=(ADDR1,"a")).start()
    Thread(target = receiveData,args=(ADDR2,)).start()