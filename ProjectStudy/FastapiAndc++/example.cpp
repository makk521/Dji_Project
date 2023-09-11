// example.cpp
#include <iostream>

extern "C" {
    void hello() {
        std::cout << "Hello from C++" << std::endl;
    }

    void world(){
        std::cout << "World from C++" << std::endl;
    }
}
