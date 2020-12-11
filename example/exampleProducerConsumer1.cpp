#include <thread>
#include <chrono>
#include <iostream>
using std::endl;
using std::cout;
#include "rtb/concurrency/Concurrency.h"
using namespace rtb::Concurrency;

// In this example, one producer and one consumer are defined as free functions. A `Queue` is
// defined as global variable and used as shared memory area to pass the data between the two
// threads. The access the data internal to the `Queue` is automatically managed, is thread safe and
// data race free. An `endToken` is defined to notify that the producer has finished producing data
// and the consumer can gracefully close.

// Define the queue as global variable
Queue<int> q;
constexpr int endToken = std::numeric_limits<int>::max();

void produce(int n) {
    for (int i{ 0 }; i < n; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        cout << "Producer (id#" << std::this_thread::get_id() << "): " << i << endl;
        q.push(i);
    }
    q.push(endToken);
}

void consume() {
    // Important, you always need to subscribe to the `Queue` prior reading from it
    q.subscribe();
    bool run = true;
    while (run) {
        int value = q.pop();
        if (value != endToken) {
            cout << "Consumer (id#" << std::this_thread::get_id() << "): " << value << endl;
        } else {
            run = false;
        }
    }
    // When no consumers are subscribet to the `Queue`, data is removed from the Queue and
    // no new data is added until a new consumer subscribes
    q.unsubscribe();
}
int main() {
    // Run `produce` function on a new thread providing a function argument
    std::thread prodThr(&produce, 10);
    // Run `consume` function on a new thread
    std::thread consThr(&consume);

    // Wait for the threads to terminate before exiting the main function
    prodThr.join();
    consThr.join();

    return 0;
}