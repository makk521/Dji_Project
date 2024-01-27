cmake_minimum_required(VERSION 2.8)  # 指定最低cmake版本

project(easylogTest) # 设置project的名称，后面PROJECT_NAME就会被改成这个
add_definitions(-DELPP_DEFAULT_LOGGER=\"myLogger\")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g -O0") # 设置cmake的FLAGS信息

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
) # 设置头文件的搜索路径

file(GLOB_RECURSE SRC_FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.cc)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin) 

add_executable(${PROJECT_NAME} ${SRC_FILES})
