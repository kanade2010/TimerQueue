cmake_minimum_required (VERSION 2.8.7)
project (timer_queue_test)

###
# variables
###
set(LOGGER_INCLUDES ${PROJECT_SOURCE_DIR}/Logger/)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/)       #设置可执行文件的输出目录

set(SOURCE_FILES                                            #源代码文件
    timer_queue_test.cpp
    Channel.cpp
    EventLoop.cpp
    Poller.cpp
    TimerQueue.cpp
    Logger/Logger.cpp
    Logger/LogStream.cpp
    Logger/TimeStamp.cpp
    )

###
# includes
###
include_directories(${LOGGER_INCLUDES} ${PROJECT_SOURCE_DIR})

###
# libraries
###
#link_directories("${####}")

###
# compilation options
###
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra -O3")

add_executable(timer_queue_test ${SOURCE_FILES})
target_link_libraries(timer_queue_test pthread)
