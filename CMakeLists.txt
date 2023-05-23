cmake_minimum_required(VERSION 3.23)
project(ProtobufParser)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(SOURCE_DIR)

add_subdirectory(test)
enable_testing()