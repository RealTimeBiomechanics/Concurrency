#include <thread>
#include <chrono>
#include <iostream>
using std::endl;
using std::cout;
#include "rtb/concurrency/Concurrency.h"
using namespace rtb::Concurrency;
constexpr int endToken = std::numeric_limits<int>::max();

// This example uses structs, rather than free functions, to implement producers and consumers

struct Producer {
    Producer(int n, Queue<int> &outputQueue)
        : numberOfMessages_(n)
        , outputQueue_(outputQueue) {}
    // The `operator()()` is the default function that is run when the struct is moved to a new
    // thread
    void operator()() {
        for (int i{ 0 }; i < numberOfMessages_; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            cout << "Producer (id#" << std::this_thread::get_id() << "): " << i << endl;
            outputQueue_.push(i);
        }
        outputQueue_.push(endToken);
    }
    int numberOfMessages_;
    Queue<int> &outputQueue_;
};

struct Consumer {
    Consumer(Queue<int> &inputQueue)
        : inputQueue_(inputQueue) {}
    void operator()() {
        inputQueue_.subscribe();
        bool run = true;
        while (run) {
            int value = inputQueue_.pop();
            if (value != endToken) {
                cout << "Consumer (id#" << std::this_thread::get_id() << "): " << value << endl;
            } else {
                run = false;
            }
        }
        inputQueue_.unsubscribe();
    }
    Queue<int> &inputQueue_;
};

int main() {
    // Define the queue as a variable in the main scope
    Queue<int> q;

    Producer prod(10, q);
    Consumer cons(q);

    // Run `produce` function on a new thread providing a function argument
    std::thread prodThr(std::ref(prod));
    // Run `consume` function on a new thread
    std::thread consThr(std::ref(cons));

    // Wait for the threads to terminate before exiting the main function
    prodThr.join();
    consThr.join();

    return 0;
}