cmake_minimum_required(VERSION 3.31)
project(byte_array_ops VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} STATIC
        src/byte_array_ops.cpp
        src/security_ops.cpp)

target_include_directories(${PROJECT_NAME}
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src")

