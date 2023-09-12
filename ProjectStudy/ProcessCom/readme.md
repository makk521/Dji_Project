## 目标

python通过socket将数据传给c++


## 测试

流程为云服务器端运行Post.py，将数据推送到公网中。PostSocket.py请求该地址数据并通过socket将数据出演递给c++，需要保证ReceiveSocket先运行(主机)

## linux运行c++

1. 打开终端并进入包含您的C++源代码文件的目录。
2. 使用以下命令编译服务器程序：

```
g++ ReceiveSocket.cpp -o ReceiveSocket
```

这将编译 `ReceiveSocket.cpp`并生成一个名为 ReceiveSocket的可执行文件。

3. 运行服务器程序：

```
./ReceiveSocket
```
