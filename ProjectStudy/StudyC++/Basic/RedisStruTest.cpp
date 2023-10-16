#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

template <typename T>
json structToJson(const T& obj) {
    json jsonObj;

    jsonObj["x"] = obj.x;
    jsonObj["y"] = obj.y;
    jsonObj["z"] = obj.z;

    return jsonObj;
}

struct Vector3f {
    float x;
    float y;
    float z;
};

struct Person {
    std::string name;
    int age;
};

int main() {
    Vector3f myVector = {1.0f, 2.0f, 3.0f};
    Person person = {"John", 30};

    json vectorJson = structToJson(myVector);
    json personJson = structToJson(person);

    std::cout << "Vector JSON: " << vectorJson.dump(4) << std::endl;
    std::cout << "Person JSON: " << personJson.dump(4) << std::endl;

    return 0;
}
