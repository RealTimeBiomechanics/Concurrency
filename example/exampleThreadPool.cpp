#include "rtb/concurrency/Queue.h"
#include "rtb/concurrency/Latch.h"
#include "rtb/concurrency/ThreadPool.h"
#include <chrono>
#include <iostream>
#include <random>
#include <chrono>
using namespace rtb::Concurrency;

std::random_device rd;// Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd());// Standard mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<> distrib(200, 3000);

struct AddOne {
    using InputData = double;
    using OutputData = double;
    double operator()(double value) {
        std::this_thread::sleep_for(std::chrono::milliseconds(distrib(gen)));
        std::cout << "Worker (#" << std::this_thread::get_id() << "): processing value " << value
                  << std::endl;
        return value + 1.1;
    }
};

struct Source {
    Source(Queue<double> &outputQueue)
        : outputQueue_(outputQueue) {}
    void operator()() {
        int val = 0;
        for(int i{0}; i < 7; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(distrib(gen)));
            outputQueue_.push(val++);
        }
        outputQueue_.close();
    }

  private:
    Queue<double> &outputQueue_;
};

struct Sink {
    Sink(Queue<double> &inputQueue)
        : inputQueue_(inputQueue) {}
    void operator()() {
        inputQueue_.subscribe();
        while (true) {
            auto val{ inputQueue_.pop() };
            if (!val.has_value()) break;
            data_.push_back(val.value());
            std::cout << "Sink: " << val.value() << std::endl;
        }
        inputQueue_.unsubscribe();
    }
    void print() {
        for (auto e : data_) {
            std::cout << e << std::endl;
        }
    }

  private:
    Queue<double> &inputQueue_;
    std::vector<double> data_;
};

int main() {
    AddOne adder;
    Queue<double> inputQueue, outputQueue;
    Source source(inputQueue);
    Sink sink(outputQueue);
    auto pool(makeExecutionPool(inputQueue, outputQueue, 9));
    std::thread thSource(std::ref(source));
    std::thread thSink(std::ref(sink));
    std::thread thPool(std::ref(*pool), adder);

    thSource.join();
    thSink.join();
    thPool.join();
    sink.print();

    return 0;
}