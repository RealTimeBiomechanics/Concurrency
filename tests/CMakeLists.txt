add_executable(latch latchMain.cpp)
target_link_libraries(latch Concurrency Threads::Threads)

add_executable(queue queueMain.cpp threadFunctions.h threadFunctions.cpp)
target_link_libraries(queue Concurrency Threads::Threads)
