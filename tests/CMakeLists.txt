include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${Concurrency_INCLUDE_DIR})

add_executable(latch latchMain.cpp)
target_link_libraries(latch Concurrency)

add_executable(queue queueMain.cpp threadFunctions.h threadFunctions.cpp)
