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
#include "threadFunctions.h"
#include "rtb/concurrency/Queue.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

std::mutex outputMutex;

using namespace rtb::Concurrency;

// all times in milliseconds

void Producer::operator()(Queue<int> &q, TimeT startTime, TimeT period, int noMessages) {
    auto start = std::chrono::system_clock::now();
    producedValues.reserve(200);
    std::this_thread::sleep_for(startTime);

    for (int i = 1; i <= noMessages; ++i) {
        q.push(i);
        producedValues.push_back(i);
        std::this_thread::sleep_for(period);
    }
    q.push(std::numeric_limits<int>::max());
}

void Consumer::operator()(Queue<int> &q,
    const int id,
    const std::vector<TimeT> &subscribeTime,
    const std::vector<TimeT> &unsubscribeTime,
    TimeT readingCycleTime) {
    bool lastMessage = false;
    unsigned nextConsumerPeriod = 0;

    auto start = std::chrono::system_clock::now();
    consumedValues.reserve(200);
    std::this_thread::sleep_for(subscribeTime.at(nextConsumerPeriod));

    // start time for consumers and producer are not exactly the same
    // but for test purposes is ok

    do {
        q.subscribe();

        bool timeToUnsubscribe = false;

        do {
            std::this_thread::sleep_for(readingCycleTime);
            auto item = q.pop().value();
            lastMessage = (item == std::numeric_limits<int>::max());
            if (!lastMessage) consumedValues.push_back(item);

            timeToUnsubscribe =
                (unsubscribeTime.at(nextConsumerPeriod)
                    <= std::chrono::duration_cast<TimeT>(std::chrono::system_clock::now() - start));

        } while (!lastMessage && !timeToUnsubscribe);
        // std::cout << "BEFORE:\n" << q << std::endl;
        q.unsubscribe();
        //  std::cout << "AFTER:\n" << q << std::endl;
        nextConsumerPeriod++;
        if (nextConsumerPeriod < subscribeTime.size()) {
            std::this_thread::sleep_for(
                subscribeTime.at(nextConsumerPeriod) - unsubscribeTime.at(nextConsumerPeriod - 1));
        }
    } while ((!lastMessage) && (nextConsumerPeriod < subscribeTime.size()));

    std::unique_lock<std::mutex> mlock(outputMutex);
    std::cout << "Consumer " << id << " (" << std::this_thread::get_id()
              << ")  popped the following values: \n";
    for (auto &it : consumedValues)
        std::cout << it << " ";
    std::cout << std::endl;
    mlock.unlock();
}