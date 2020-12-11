#include <thread>
#include <chrono>
#include <iostream>
using std::endl;
using std::cout;
#include "rtb/concurrency/Concurrency.h"
using namespace rtb::Concurrency;

constexpr int endToken = std::numeric_limits<int>::max();

// Now that the queue is not defined as global variable, producer needs to know where to push the
// data. A non-const reference is passed as function argument.
void produce(int n, Queue<int> &q) {
    for (int i{ 0 }; i < n; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        cout << "Producer (id#" << std::this_thread::get_id() << "): " << i << endl;
        q.push(i);
    }
    q.push(endToken);
}

// Now that the queue is not defined as global variable, consumer needs to know from where to read the
// data. A non-const reference is passed as function argument, as data is removed from the queue after reading
void consume(Queue<int> &q) {
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

// Here a single produces generates mnumber from 0 to 10. These are read by 2 independent consumers.
// Each consumer reads a full copy of the data generated by the producer
int main() {
    // Define the queue as a variable in the main scope
    Queue<int> q;
    // Run `produce` function on a new thread providing a function argument
    std::thread prodThr(&produce, 10, std::ref(q));
    // Run `consume` function on a new thread
    std::thread consThr1(&consume, std::ref(q));
    std::thread consThr2(&consume, std::ref(q));

    // Wait for the threads to terminate before exiting the main function
    prodThr.join();
    consThr1.join();
    consThr2.join();

    return 0;
}