add_executable(testLatch testLatch.cpp)
target_link_libraries(testLatch Concurrency)
add_test(TestLatch testLatch)

add_executable(testQueue testQueue.cpp threadFunctions.h threadFunctions.cpp)
target_link_libraries(testQueue Concurrency)
add_test(TestQueue testQueue)

