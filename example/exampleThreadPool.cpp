#include "rtb/concurrency/Queue.h"
#include "rtb/concurrency/Latch.h"
#include "rtb/concurrency/ThreadPool.h"
#include <chrono>
#include <iostream>
#include <type_traits>

using namespace rtb::Concurrency;
struct AddOne {
    using InputData = int;
    using OutputData = int;
    int operator()(int value) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return value + 1;
    }
};

struct Source {
    Source(Queue<int> &outputQueue)
        : outputQueue_(outputQueue) {}
    void operator()() {
        int val = 0;
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            outputQueue_.push(val++);
        }
    }

  private:
    Queue<int> &outputQueue_;
};

struct Sink {
    Sink(Queue<int> &inputQueue)
        : inputQueue_(inputQueue) {}
    void operator()() {
        inputQueue_.subscribe();
        while (true) {
            std::cout << inputQueue_.pop() << std::endl;
        }
    }

  private:
    Queue<int> &inputQueue_;
};

int main() {
    AddOne adder;
    Queue<int> inputQueue, outputQueue;
    Source source(inputQueue);
    Sink sink(outputQueue);
    auto pool(makeExecutionPool(inputQueue, outputQueue, 3));
    std::thread thSource(std::ref(source));
    std::thread thSink(std::ref(sink));
    std::thread thPool(std::ref(*pool), adder);

    thSource.join();
    thSink.join();
    thPool.join();
    return 0;
}