#include <thread>
#include <chrono>
#include <iostream>
using std::endl;
using std::cout;
#include "rtb/concurrency/Concurrency.h"
using namespace rtb::Concurrency;

// This example uses the class `Latch` to synchronise the execution of different threads

struct Producer {
    Producer(int n, Queue<int> &outputQueue, Latch &latch)
        : numberOfMessages_(n)
        , outputQueue_(outputQueue)
        , latch_(latch) {}
    // The `operator()()` is the default function that is run when the struct is moved to a new
    // thread
    void operator()() {
        // wait on the latch until all other threads get to the latch
        cout << "Producer (id#" << std::this_thread::get_id() << "): "
             << " latch waiting" << endl;
        latch_.wait();
        cout << "Producer (id#" << std::this_thread::get_id() << "): "
             << " latch released" << endl;
        
        for (int i{ 0 }; i < numberOfMessages_; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            cout << "Producer (id#" << std::this_thread::get_id() << "): " << i << endl;
            outputQueue_.push(i);
        }
        outputQueue_.close();
    }
    int numberOfMessages_;
    Queue<int> &outputQueue_;
    Latch &latch_;
};

struct Consumer {
    Consumer(Queue<int> &inputQueue, Latch &latch)
        : inputQueue_(inputQueue)
        , latch_(latch) {}
    void operator()() {
        inputQueue_.subscribe();
        bool run = true;
        // perform some complex initialisation. We simulate that with a sleep function.
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        // This thread will wait on the latch until all other threads also reach the `Latch`.
        // In this particular situation, this is likely the last thread to reach the `Latch`, thus
        // unlocking all other threads currently waiting on the `Latch`
        cout << "Consumer (id#" << std::this_thread::get_id() << "): "
             << " latch waiting" << endl;
        latch_.wait();
        cout << "Consumer (id#" << std::this_thread::get_id() << "): "
             << " latch released" << endl;
        while (inputQueue_.isOpen()) {
            int value = inputQueue_.pop();
            cout << "Consumer (id#" << std::this_thread::get_id() << "): " << value << endl;
        }
        inputQueue_.unsubscribe();
    }
    Queue<int> &inputQueue_;
    Latch &latch_;
};

int main() {
    // Define the queue as a variable in the main scope
    Queue<int> q;
    // Set the value of `latch` to 2, as 2 different threads will run using a `Latch`
    Latch latch(2);
    Producer prod(10, q, latch);
    Consumer cons(q, latch);

    // Run `produce` function on a new thread providing a function argument
    std::thread prodThr(std::ref(prod));
    // Run `consume` function on a new thread
    std::thread consThr(std::ref(cons));

    // Wait for the threads to terminate before exiting the main function
    prodThr.join();
    consThr.join();

    return 0;
}