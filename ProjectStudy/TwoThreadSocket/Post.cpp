// 天翼云向腾讯云发
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

void postData(int clientSocket, sockaddr_in serverAddr){
    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        close(clientSocket);
        return ;
    }
    // 持续发送数据
    const char* message = "Hello from client";
    while (true) {
        int bytesSent = send(clientSocket, message, strlen(message), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            break;
        }
        std::cout << "Sent data: " << message << std::endl;

        // 添加适当的延时，以控制发送速率
        usleep(3000000); // 休眠 1 秒
    }
    
}

int main() {
    // 创建 Socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        return 1;
    }

    // 设置服务器的 IP 地址和端口号
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000); // 替换为服务器的端口号
    serverAddr.sin_addr.s_addr = inet_addr("124.223.76.58"); // 目标服务器IP

    std::thread posterThread(postData, clientSocket, serverAddr);

    posterThread.join();
    

    // 关闭 Socket
    close(clientSocket);

    return 0;
}
