/**
* @file dataPackege.cpp
* @author Makaka
* @date 2023-11-19
* @brief c++11实现线程安全队列,定义10个结构体放在vector中，队列操作时使用指针。
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue> 

struct Student {
    std::string name;
    int age;
};

/**
* @brief 模板函数重写push与pop函数，用于多线程保护
* @param None
* @return None
*/
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

void producerFun(ThreadSafeQueue<Student*>& sharedQueue,std::vector<Student>& students) { 
    /**
    * @brief 生产者函数，将vector中的数据放入队列中,存入每个数据的指针
    * @param sharedQueue 数据存储队列  students 数据源
    * @return None
    */
    for (auto& student : students) {
        Student* ptr = &student;
        sharedQueue.push(ptr);  
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumerFun(ThreadSafeQueue<Student*>& sharedQueue) { 
    /**
    * @brief 消费者函数，从队列中取出数据(指针)
    * @param sharedQueue 数据存储队列
    * @return None
    */
    for (int i = 0; i < 10; ++i) {
        Student* ptr = sharedQueue.pop();  
        std::cout << "Consumed: " << "Name: " << ptr->name << ", Age: " << ptr->age << std::endl;
    }
}

int main() {
    std::vector<Student> students;
    for (int i = 0; i < 10; ++i) {
        Student student;
        student.name = "Student " + std::to_string(i);
        student.age = 20 + i;
        students.push_back(student);
    }

    for (auto& student : students) {
        Student* ptr = &student;  // 获取每个元素的地址
        std::cout << "Pointer :" << ptr << std::endl;
        // 使用ptr进行进一步操作
        std::cout << "Name: " << ptr->name << ", Age: " << ptr->age << std::endl;
    }

    ThreadSafeQueue<Student*> sharedQueue;

    std::thread producerThread(producerFun, std::ref(sharedQueue), std::ref(students));
    std::thread consumerThread(consumerFun, std::ref(sharedQueue));

    producerThread.join();
    consumerThread.join();

    return 0;
}
