#include "RedisConnect.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	string val;
 
	//初始化连接池
	RedisConnect::Setup("124.223.76.58", 6379);
 
	//从连接池中获取一个连接
	shared_ptr<RedisConnect> redis = RedisConnect::Instance();
 
	//设置一个键值
	redis->set("ma", "kaka");
	cout << "插入成功" << endl;
	//获取键值内容
	cout << redis->get("ma") << endl;
 
 
	return 0;
}
