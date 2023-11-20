#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>

template <typename T, typename Compare = std::less<typename std::vector<T>::value_type>>
class PriorityBlockingQueue {
public:
    PriorityBlockingQueue() = default;

    void push(const T& value) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(value);
        lock.unlock();
        condition_variable.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex);
        condition_variable.wait(lock, [this] { return !queue.empty(); });

        T value = queue.top();
        queue.pop();
        return value;
    }

    bool empty() const {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }

private:
    std::priority_queue<T, std::vector<T>, Compare> queue;
    mutable std::mutex mutex;
    std::condition_variable condition_variable;
};

int main() {
    PriorityBlockingQueue<int, std::greater<int>> priorityQueue;

    // Enqueue elements with different priorities
    priorityQueue.push(5);
    priorityQueue.push(2);
    priorityQueue.push(8);

    // Dequeue elements (higher priority elements will be dequeued first)
    while (!priorityQueue.empty()) {
        std::cout << priorityQueue.pop() << std::endl;
    }

    return 0;
}
