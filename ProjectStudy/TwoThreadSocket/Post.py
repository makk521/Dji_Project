'''
Local Pc 向腾讯云发送数据
'''
import socket
import sys
import time
from threading import Thread


def sock_client_data(address,send_data):
    """
    Connect to cloud and send data(every three seconds)

    Arguments:
    address    - ('cloud public mac', port)
    send_data  
    """
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(address)
    except socket.error as msg:
        print(msg)
        print(sys.exit(1))

    while True:
        s.send(send_data.encode('utf-8'))  # 将要传输的数据编码发送，如果是字符数据就必须要编码发送
        time.sleep(3)


if __name__ == '__main__':
    ADDR1 = ('124.223.76.58', 5000) # Tencent
    # ADDR1 = ('202.38.68.70', 5000) # ustc privat

    Thread(target = sock_client_data,args=(ADDR1,"a")).start()
