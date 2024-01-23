cmake_minimum_required(VERSION 2.8)  # 指定最低cmake版本

project(spdlogTest) # 设置project的名称，后面PROJECT_NAME就会被改成这个

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g -O0") # 设置cmake的FLAGS信息
add_definitions(-DDEBUG)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
) # 设置头文件的搜索路径

file(GLOB_RECURSE SRC_FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin) 

add_executable(${PROJECT_NAME} ${SRC_FILES})
