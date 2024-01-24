#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

void initLogger(){
    /**
    * @brief 配置名为"myLogger"的日志记录器，使其将所有级别的日志消息写入到文件"logs/myLoggerFile.log"中
    * @param None
    * @return None
    */
    el::Configurations conf;
    conf.setToDefault();
    conf.set(el::Level::Global, el::ConfigurationType::Filename, "logs/myLoggerFile.log");
    el::Loggers::reconfigureLogger("myLogger", conf);
}

int main() {
    
    initLogger();
    LOG(INFO) << "This is an info log";
    LOG(DEBUG) << "This is a debug log";
    LOG(WARNING) << "This is a warning log";
    LOG(ERROR) << "This is an error log";
    LOG(FATAL) << "This is a fatal log";

    return 0;
}