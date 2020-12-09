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
#ifndef threadFunctions_h
#define threadFunctions_h

#include "rtb/concurrency/Queue.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

extern std::mutex outputMutex;

typedef std::chrono::milliseconds TimeT;
// all times in milliseconds
struct Producer {
    void operator()(rtb::Concurrency::Queue<int> &q, TimeT startTime, TimeT period, int noMessages);
    std::vector<int> producedValues;
};

struct Consumer {
    void operator()(rtb::Concurrency::Queue<int> &q,
        const int id,
        const std::vector<TimeT> &subscribeTime,
        const std::vector<TimeT> &unsubscribeTime,
        TimeT readingCycleTime);
    std::vector<int> consumedValues;
};
#endif
