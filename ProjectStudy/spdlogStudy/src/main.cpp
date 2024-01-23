#include "spdlog/spdlog.h"
#include <chrono>
#include <thread>
#include "spdlog/sinks/basic_file_sink.h"
// trace = SPDLOG_LEVEL_TRACE // 最低级（用来记录代码执行轨迹）
// debug = SPDLOG_LEVEL_DEBUG //      （用来记录debug信息）
// info = SPDLOG_LEVEL_INFO   // 在上面的测试例子中用过
// warn = SPDLOG_LEVEL_WARN
// err = SPDLOG_LEVEL_ERROR
// critical = SPDLOG_LEVEL_CRITICAL
// off = SPDLOG_LEVEL_OFF     // 最高级

auto my_logger = spdlog::basic_logger_mt("file_logger", "logs/basic-log.txt");

int main()
{
    my_logger->set_level(spdlog::level::trace); // 默认是info级别
    my_logger->flush_on(spdlog::level::trace); // 刷新级别是trace

    my_logger->trace("Some trace message");
    my_logger->debug("Some debug message");
    my_logger->info("Some info message");
    my_logger->warn("Some warn message");
    my_logger->error("Some error message");
    my_logger->critical("Some critical message");

    for (int i = 0; i < 10; ++i) {
        spdlog::info("The current value is: {}", i);
        my_logger->info("The current value is: {}", i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    return 0;
}
