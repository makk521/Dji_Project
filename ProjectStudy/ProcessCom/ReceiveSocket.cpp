#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char buffer[1024];

    // 创建Socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // 设置服务器地址
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888); // 选择一个空闲端口
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定Socket
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // 监听连接请求
    listen(serverSocket, 5);

    std::cout << "Server listening on port 8888..." << std::endl;

    // 接受客户端连接
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);

    // 接收来自客户端的数据
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);

    std::cout << "Received from Python client: " << buffer << std::endl;

    // 关闭Socket连接
    // close(clientSocket);
    // close(serverSocket);

    return 0;
}
