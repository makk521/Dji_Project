#include "Mobility.hpp"
#include <cerrno>

#define SERVER_IP "127.0.0.1"//服务端ip,这里是本机
#define MY_PORT 1234//端口号
#define BUF_SIZE 1024//最大缓存
#define QUEUE 20//最大连接数

string Redis_IP = "172.21.2.7";
int Redis_Port = 6379;
string Redis_Password = "Ustc1958@2023";
int Redis_ID = 11;

/*--格式转换相关函数--*/
json StructToJson(const UAV_mobility& obj) {
    json jsonObj;
    jsonObj["uav_id"] = obj.uav_id;
    jsonObj["uav_ip"] = obj.uav_ip;
    jsonObj["cluster_id"]= obj.cluster_id;
    return jsonObj;
}
string JsonToString(const json& obj){
    return obj.dump();
}
json StringToJson(const string& obj){
    return nlohmann::json::parse(obj);
}
UAV_mobility JsonToStruct(json jsonObj){
    UAV_mobility u;
    u.uav_id = jsonObj["uav_id"];
    u.uav_ip = jsonObj["uav_ip"];
    u.cluster_id = jsonObj["cluster_id"];
    return u;
}

/*--获取无人机信息--*/
UAV_mobility get_UAV_info(){
	UAV_mobility uav = {1,"192.168.0.3",2};
    cout<<"[Mobility]读取配置文件-开始"<<endl;
    json data;  
    
    ifstream file("/home/dji/Desktop/Onboard-SDK/sample/platform/linux/flight-control/uav_info.json",ios::in);
    if(!file.is_open()) cout<<"打开文件失败,error:"<<strerror(errno)<<endl;
    file.clear();
    file.seekg(0,ios::beg);
    data = nlohmann::json::parse(file);
    file.clear();
    file.seekg(0,ios::beg);
    file.close();

    uav.uav_id=data["uav_id"];
    uav.cluster_id=data["cluster_id"];
    uav.uav_ip=data["uav_ip"];

    cout<<"[Mobility]读取配置文件-完成"<<endl;
    
	return uav;
}

/*--数据库操作部分--*/
//连接无人机和服务器，这里暂时采用redisconnect的连接方式，之后改成socket做测试，并等待信息模块的接口
shared_ptr<RedisConnect> ConnectToRedis(int redis_id){
	RedisConnect::Setup(Redis_IP, Redis_Port , Redis_Password);
    shared_ptr<RedisConnect> redis = RedisConnect::Instance();
    redis->select(redis_id);
	return redis;
}
//向Redis写入数据
void Write_UAVinfo(int key_int, string value){
    string key = to_string(key_int);
	shared_ptr<RedisConnect> redis = ConnectToRedis(Redis_ID);
	redis->set(key, value);
	cout << "[Mobility]更新无人机信息：（" <<key << "," << value<< "）"<< endl;
}
//从Redis中读取数据
string Read_UAVinfo(int uavID){
    string key = to_string(uavID);
    shared_ptr<RedisConnect> redis = ConnectToRedis(Redis_ID);
	string jsonString=redis->get(key);
	//cout << "[Mobility]读取无人机信息：（" << key << "," << jsonString<< "）"<<endl;
    return jsonString;
}
//读取uavID对应的IP信息
string Read_UAVinfo_IP(int uavID){
	string ReadJsonString = Read_UAVinfo(uavID);
    json ReadUavJson = nlohmann::json::parse(ReadJsonString);//反序列化
    //cout << "[Mobility]ID为" << uavID <<"的无人机对应的IP地址为：" << ReadUavJson["uav_ip"] <<endl;
	return ReadUavJson["uav_ip"];
}

/*--与信息模块交互--*/
void Pack_mobility(string uav_info,DataPack *dp){
    int len=uav_info.length();
    dp->setPackType(9);//设置包类型
    dp->setPackLength(224+(len+1)*8);//设置包长度，单位为bit
    //dp->setPackLength(len+1);//设置包长度，单位为byte
    //dp->payload = (char*)uav_info.c_str();
    dp->payload = new char[len+1];
    strcpy(dp->payload,uav_info.c_str());
    cout<<dp->payload<<endl;
}

/*--定时更新ip信息（无人机端）--*/
void IPupdate_SendToServer(ThreadSafeQueue<DataPack>& Queue){
    UAV_mobility uav = get_UAV_info();
    json uav_json = StructToJson(uav);
    string uav_string = JsonToString(uav_json);
    DataPack *dp;
    dp = new DataPack();
    Pack_mobility(uav_string,dp);
    cout<<dp->payload<<endl;
    Queue.push(*dp);
    //delete [] dp->payload;
    //delete [] dp;
}
void IPupdate_Time(ThreadSafeQueue<DataPack>& Queue){
    while(1){
        IPupdate_SendToServer(Queue);
        this_thread::sleep_for(chrono::seconds(10));
    }
}

/*--定时更新ip信息（服务器端）--*/
void IPupdate_SaveToRedis(DataPack uav_info){
    string str  = uav_info.payload;
    //int uav_id = uav_info.getSourceId();
    //int uav_id=nlohmann::json::parse(uav_info.payload)["uav_id"];
    int uav_id = 1;
	Write_UAVinfo(uav_id,str);
}


/*--socket，测试用--*/
DataPack socket_server_datapack(){
    DataPack p;
    p.payload=NULL;

	int server_sockfd = socket(AF_INET,SOCK_STREAM,0);//建立响应socket

    struct sockaddr_in server_sockaddr;//保存本地地址信息
    server_sockaddr.sin_family = AF_INET;//采用ipv4
    server_sockaddr.sin_port = htons(MY_PORT);//指定端口
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);//获取主机接收的所有响应

    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1){//绑定本地ip与端口
        perror("Bind Failure\n");
        printf("Error: %s\n",strerror(errno));//输出错误信息
        return p;
    }
    printf("Listen Port : %d\n",MY_PORT);
    if(listen(server_sockfd,QUEUE) == -1){//设置监听状态
        perror("Listen Error");
        return p;
    }
    char buffer[BUF_SIZE];//一次传输的数据缓存
    struct sockaddr_in client_addr;//保存客户端地址信息
    socklen_t length = sizeof(client_addr);//需要的内存大小

    printf("Waiting for connection!\n");

    int connect_fd = accept(server_sockfd,(struct sockaddr *)&client_addr,&length);//等待连接，返回服务器端建立连接的socket
    if(connect_fd == -1){//连接失败
        perror("Connect Error");
        return p;
    }
    printf("Connection Successful\n");
    memset(buffer,0,sizeof(buffer));
    int len = recv(connect_fd,buffer,sizeof(buffer),0);//接收数据
    printf("client send message: %s\n",buffer);

    len = strlen(buffer);
    p.payload = new char [len+1];
    strcpy(p.payload,buffer);

    strcpy(buffer,"successful");
    send(connect_fd,buffer,strlen(buffer),0);//发送数据
    printf("send message: %s\n",buffer);
    close(connect_fd);//关闭数据socket
    close(server_sockfd);//关闭响应socket
    printf("close\n");
    return p;
}
void socket_client_datapack(const DataPack &p){
	int client_sockfd = socket(AF_INET,SOCK_STREAM,0);//建立客户端socket

    struct sockaddr_in servaddr;//保存服务器端地址信息
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;//ipv4协议
    servaddr.sin_port = htons(MY_PORT);//端口
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);//ip地址

    printf("connect to %s:%d\n",SERVER_IP,MY_PORT);
    int connect_fd = connect(client_sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));//建立连接
    if(connect_fd<0){
        perror("Connect Error");
        _exit(1);
    }
    printf("Connect Successful\n");
    char sendbuf[BUF_SIZE];
    char recvbuf[BUF_SIZE];
    cout<<p.payload<<endl;
    strcpy(sendbuf,p.payload);//发送payload中的数据
    memset(recvbuf,0,sizeof(recvbuf));
    printf("send message:%s\n",sendbuf);
    send(client_sockfd,sendbuf,strlen(sendbuf),0);
    int len = recv(client_sockfd,recvbuf,sizeof(recvbuf),0);
    if(len<=0) printf("receive failure");
    printf("recv message:%s\n",recvbuf);
    memset(sendbuf,0,sizeof(sendbuf));
    close(client_sockfd);//关闭客户端socket
    printf("close\n");
}