/*
本地运行，模板函数实现结构体转Json，实际使用重载函数
*/
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Vector3f {
    float x;
    float y;
    float z;
};

struct Person {
    std::string name;
    int age;
};

// 通用的结构体转换为 JSON 的模板函数
template <typename T>
json structToJson(const T& obj) {
    json jsonObj;
    jsonObj["Unknown Type"] = "Cannot Convert to JSON";
    return jsonObj;
}

// 模板特化版本，用于将 Vector3f 结构体转换为 JSON
template <>
json structToJson(const Vector3f& obj) {
    json jsonObj;
    jsonObj["x"] = obj.x;
    jsonObj["y"] = obj.y;
    jsonObj["z"] = obj.z;
    return jsonObj;
}

// 模板特化版本，用于将 Person 结构体转换为 JSON
template <>
json structToJson(const Person& obj) {
    json jsonObj;
    jsonObj["name"] = obj.name;
    jsonObj["age"] = obj.age;
    return jsonObj;
}

int main() {
    Vector3f myVector = {1.0f, 2.0f, 3.0f};
    Person person = {"John", 30};

    json vectorJson = structToJson(myVector);
    json personJson = structToJson(person);

    std::cout << "Vector JSON: " << vectorJson.dump(4) << std::endl;
    std::cout << "Person JSON: " << personJson.dump(4) << std::endl;

    json myJson;
    myJson["vectorJson"] = vectorJson;
    myJson["personJson"] = personJson;
    std::string jsonString = myJson.dump();
    std::cout << jsonString << std::endl;

    return 0;
}
