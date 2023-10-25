#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>  // Include queue header

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
    std::queue<T> queue;  // Use std::queue to store data
    std::mutex mutex;
    std::condition_variable condition_variable;
};

void producerFun(ThreadSafeQueue<int>& sharedQueue) {  // Change the argument type to ThreadSafeQueue
    for (int i = 0; i < 9; ++i) {
        sharedQueue.push(i);  // Use push method
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumerFun(ThreadSafeQueue<int>& sharedQueue) {  // Change the argument type to ThreadSafeQueue
    for (int i = 0; i < 10; ++i) {
        int value = sharedQueue.pop();  // Use pop method
        std::cout << "Consumed: " << value << std::endl;
    }
}

int main() {
    ThreadSafeQueue<int> sharedQueue;

    std::thread producerThread(producerFun, std::ref(sharedQueue));
    std::thread consumerThread(consumerFun, std::ref(sharedQueue));

    producerThread.join();
    consumerThread.join();

    return 0;
}
