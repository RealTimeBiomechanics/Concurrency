project(queue)

include_directories(lib/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")

add_subdirectory(tests/)
