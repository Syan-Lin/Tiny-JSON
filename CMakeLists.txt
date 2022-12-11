cmake_minimum_required(VERSION 3.10)

project(tiny_json)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_STANDARD 17)
endif()

# 非作为第三方库，则编译样例程序及测试
if(CMAKE_PROJECT_NAME STREQUAL "tiny_json")
    add_executable(example examples.cpp)
    enable_testing()
    add_subdirectory(Tests)
endif()