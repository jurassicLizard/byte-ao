cmake_minimum_required(VERSION 3.31)
project(byte-ao)

set(BYTEAO_PROJECT_NAME byte-ao)
set(CMAKE_CXX_STANDARD 20)

add_library(${BYTEAO_PROJECT_NAME} STATIC
        src/byte_array_ops.cpp
        src/security_ops.cpp
        src/byte_array.cpp)

target_include_directories(${BYTEAO_PROJECT_NAME}
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/include/public"
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include/private")

add_library(jlizard::byte-ao ALIAS ${BYTEAO_PROJECT_NAME})

enable_testing()
include(tests.cmake)


