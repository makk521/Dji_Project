#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>  // 包含 string 头文件

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;

    // 向队列中推送数据
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(value);
        condition_variable.notify_one();
    }

    // 从队列中弹出数据
    T pop() {
        std::unique_lock<std::mutex> lock(mutex);
        condition_variable.wait(lock, [this] { return !queue.empty(); });
        T value = queue.front();
        queue.pop();
        return value;
    }

private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable condition_variable;
};

void producerFun(ThreadSafeQueue<std::string>& sharedQueue) {  // 将参数类型更改为 std::string
    for (int i = 0; i < 9; ++i) {
        sharedQueue.push("String " + std::to_string(i));  // 将字符串推送到队列中
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumerFun(ThreadSafeQueue<std::string>& sharedQueue) {  // 将参数类型更改为 std::string
    for (int i = 0; i < 9; ++i) {
        std::string value = sharedQueue.pop();
        std::cout << "Consumed: " << value << std::endl;
    }
}

int main() {
    ThreadSafeQueue<std::string> sharedQueue;  // 将队列类型更改为 std::string

    std::thread producerThread(producerFun, std::ref(sharedQueue));
    std::thread consumerThread(consumerFun, std::ref(sharedQueue));

    producerThread.join();
    consumerThread.join();

    return 0;
}
