cmake_minimum_required(VERSION 2.8)  # 指定最低cmake版本

project(scsn) # 设置project的名称，后面PROJECT_NAME就会被改成这个

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g -O0") # 设置cmake的FLAGS信息

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Mobility
    ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann
    ${CMAKE_CURRENT_SOURCE_DIR}/Redis
    ${CMAKE_CURRENT_SOURCE_DIR}/Scsn
    ${CMAKE_CURRENT_SOURCE_DIR}/CurlCallback
) # 设置头文件的搜索路径

file(GLOB_RECURSE SRC_FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/Scsn/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Mobility/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CurlCallback/*.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin) 

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} curl)  # 链接 libcurl.so 动态库,系统的，不用找动态库地址 link_directories(${CMAKE_SOURCE_DIR}/lib)
