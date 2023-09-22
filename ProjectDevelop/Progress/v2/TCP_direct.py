
import time
import serial
import threading
import socket

# 串口配置
SERIAL_PORT = '/dev/ttyACM0'  # 根据实际情况修改串口地址
BAUD_RATE = 9600

# TCP配置
SERVER_IP = '192.168.10.154'  # 修改为目标设备的IP地址
SERVER_PORT = 8888

def forward_serial_to_tcp(ser, client_socket):
    try:
        while True:
            if ser.in_waiting > 0:
                data = ser.readline()
                # print('接收到的串口数据:', data)
                try:
                    client_socket.sendall(data)
                except socket.error:
                    print('与客户端的连接已断开')
                    break
    except Exception as e:
        print('发生异常:', str(e))
    finally:
        # 关闭与TCP客户端的连接
        client_socket.close()

def forward_tcp_to_serial(ser, client_socket):
    try:
        while True:
            data = client_socket.recv(2048)
            if not data:
                print('与客户端的连接已断开')
                break
            # print('接收到的TCP数据:', data)
            ser.write(data)
    except Exception as e:
        print('发生异常:', str(e))
    finally:
        # 关闭与TCP客户端的连接
        client_socket.close()

def forward_data():
    # 连接串口
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
    
    # 创建TCP服务器
    tcp_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_server.bind((SERVER_IP, SERVER_PORT))
    tcp_server.listen(1)
    print('TCP服务器已启动，等待连接...')
    
    try:
        while True:
            # 等待TCP客户端连接
            client_socket, client_address = tcp_server.accept()
            print('与客户端', client_address, '建立连接')
            
            # 创建并启动用于串口到TCP转发的线程
            serial_to_tcp_thread = threading.Thread(target=forward_serial_to_tcp, args=(ser, client_socket))
            serial_to_tcp_thread.start()
            
            # 创建并启动用于TCP到串口转发的线程
            tcp_to_serial_thread = threading.Thread(target=forward_tcp_to_serial, args=(ser, client_socket))
            tcp_to_serial_thread.start()
    except KeyboardInterrupt:
        print('程序已停止')
    finally:
        # 关闭串口、TCP服务器和客户端连接
        ser.close()
        tcp_server.close()

if __name__ == '__main__':
    forward_data()