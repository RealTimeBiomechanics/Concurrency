project(queue)
cmake_minimum_required(VERSION 2.8)
include_directories(lib/)
include_directories(tests/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")

add_subdirectory(tests/)
