/* -------------------------------------------------------------------------- *
 * Copyright (c) 2014      M. Reggiani                                        *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at:                                   *
 * http://www.apache.org/licenses/LICENSE-2.0                                 *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */
#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include "rtb/concurrency/Latch.h"

std::mutex mutexOutput;

typedef std::chrono::milliseconds TimeT;
using namespace rtb::Concurrency;

// all times in milliseconds
void doingNothing(Latch &l, int i, TimeT sleepingBeforeLatching) {
    auto start = std::chrono::system_clock::now();
    std::this_thread::sleep_for(sleepingBeforeLatching);

    auto stop = std::chrono::system_clock::now();
    std::unique_lock<std::mutex> lock1(mutexOutput);
    std::cout << "thread " << i << " entering  the barrier at: " << (stop - start).count()
              << std::endl;
    lock1.unlock();

    l.wait();

    stop = std::chrono::system_clock::now();
    std::unique_lock<std::mutex> lock2(mutexOutput);
    std::cout << "thread " << i << " went through the barrier at: " << (stop - start).count()
              << std::endl;
    lock2.unlock();
}

int main() {
    Latch l(5);

    std::thread thread1(std::bind(doingNothing, std::ref(l), 1, TimeT{ 1000 }));
    std::thread thread2(std::bind(doingNothing, std::ref(l), 2, TimeT{ 2000 }));
    std::thread thread3(std::bind(doingNothing, std::ref(l), 3, TimeT{ 3000 }));
    std::thread thread4(std::bind(doingNothing, std::ref(l), 4, TimeT{ 4000 }));
    std::thread thread5(std::bind(doingNothing, std::ref(l), 5, TimeT{ 5000 }));

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();

    Latch l2;
    l2.setCount(5);

    std::thread thread11(std::bind(doingNothing, std::ref(l2), 1, TimeT{ 1000 }));
    std::thread thread12(std::bind(doingNothing, std::ref(l2), 2, TimeT{ 2000 }));
    std::thread thread13(std::bind(doingNothing, std::ref(l2), 3, TimeT{ 3000 }));
    std::thread thread14(std::bind(doingNothing, std::ref(l2), 4, TimeT{ 4000 }));
    std::thread thread15(std::bind(doingNothing, std::ref(l2), 5, TimeT{ 5000 }));

    thread11.join();
    thread12.join();
    thread13.join();
    thread14.join();
    thread15.join();

    return 0;
}
