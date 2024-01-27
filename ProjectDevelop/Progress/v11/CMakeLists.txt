cmake_minimum_required(VERSION 2.8)  # 指定最低cmake版本

project(scsn) # 设置project的名称，后面PROJECT_NAME就会被改成这个

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g -O0") # 设置cmake的FLAGS信息
# add_definitions(-DDEBUG)
# add_definitions(-DMOBILITY_COUT)
include_directories(
    "/usr/local/include/eigen3"
    ${CMAKE_CURRENT_SOURCE_DIR}/Mobility
    ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann
    ${CMAKE_CURRENT_SOURCE_DIR}/Redis
    ${CMAKE_CURRENT_SOURCE_DIR}/Scsn
    ${CMAKE_CURRENT_SOURCE_DIR}/CurlCallback
    ${CMAKE_CURRENT_SOURCE_DIR}/Formation/commander
    ${CMAKE_CURRENT_SOURCE_DIR}/Formation/xiong_algorithm
    ${CMAKE_CURRENT_SOURCE_DIR}/easylog/
) # 设置头文件的搜索路径

file(GLOB_RECURSE SRC_FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/Scsn/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Mobility/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CurlCallback/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Formation/commander/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Formation/xiong_algorithm/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/easylog/*.cc)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin) 

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} curl)  # 链接 libcurl.so 动态库,系统的，不用找动态库地址 link_directories(${CMAKE_SOURCE_DIR}/lib)
