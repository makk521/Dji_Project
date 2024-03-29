# *  @Copyright (c) 2016-2017 DJI
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy
# * of this software and associated documentation files (the "Software"), to deal
# * in the Software without restriction, including without limitation the rights
# * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# * copies of the Software, and to permit persons to whom the Software is
# * furnished to do so, subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in
# * all copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# * SOFTWARE.
# *
# *

cmake_minimum_required(VERSION 2.8)
project(djiosdk-flightcontrol-sample)

set(USER_CONFIG_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/UserConfig.txt")

# SET(CMAKE_BUILD_TYPE "Debug") 
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
# set(CMAKE_BUILD_TYPE "Debug") 
# set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -std=c++11 -pthread -g -ggdb")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g -O0")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../core/inc)

FILE(GLOB SOURCE_FILES *.hpp *.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/dji_linux_environment.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/dji_linux_helpers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/src/flight_sample.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../hal/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../osal/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/platform/linux/flight-control/order.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/platform/linux/flight-control/dataPack.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/platform/linux/flight-control/Mobility.cpp
        ${LINKER_HEADER_SRC}
        )

if (OSDK_HOTPLUG)
    FILE(GLOB SOURCE_FILES ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/../hal/hotplug/*.c)
endif ()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})