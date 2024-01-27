#include "func.hpp"
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

void mainLog(){
    LOG(INFO) << "This is an info log in main";
    LOG(DEBUG) << "This is a debug log in main";
    LOG(WARNING) << "This is a warning log in main";
    LOG(ERROR) << "This is an error log in main";
    // LOG(FATAL) << "This is a fatal log in main";
}

int main() {
    
    initLogger();
    funLog();
    mainLog();
    

    return 0;
}