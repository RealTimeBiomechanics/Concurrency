cmake_minimum_required(VERSION 3.1.0)
project(ConcurrencyQuickStart)

find_package(Concurrency REQUIRED)

add_executable(ConcurrencyQuickStart concurrencyQuickStart.cpp)
target_link_libraries(ConcurrencyQuickStart rtb::Concurrency)