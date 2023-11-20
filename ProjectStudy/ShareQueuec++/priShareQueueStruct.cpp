#include <iostream>
#include <queue>
#include <string>

struct Student {
    std::string name;
    int age;

    // 重载 < 运算符(与std::less对应)
    bool operator<(const Student& other) const {
        return age > other.age;
    }
};

int main() {
    // 定义一个使用 Student 类型的 priority_queue，使用 std::less 来进行降序排序
    std::priority_queue<Student, std::vector<Student>, std::less<Student>> studentQueue;

    // 向队列中添加学生
    studentQueue.push({"Alice", 20});
    studentQueue.push({"Bob", 22});
    studentQueue.push({"Charlie", 18});

    // 弹出队列中的学生（按年龄升序）
    while (!studentQueue.empty()) {
        Student topStudent = studentQueue.top();
        std::cout << "Name: " << topStudent.name << ", Age: " << topStudent.age << std::endl;
        studentQueue.pop();
    }

    return 0;
}
