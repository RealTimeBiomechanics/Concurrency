/* -------------------------------------------------------------------------- *
 * Copyright (c) 2020      C. Pizzolato, M. Reggiani                          *
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

#ifndef rtb_SimpleQueue_h
#define rtb_SimpleQueue_h

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace rtb {
namespace Concurrency {
    /// SimpleQueue is an implementation of a producer consumer pattern
    /** Each message is removed from SimpleQueue after being read by any of the consumers. */
    template<typename T>
    using IndexedData = std::tuple<unsigned long long, T>;

    template<typename T>
    struct OrderByIndex {
        bool operator()(IndexedData<T> const &a, IndexedData<T> const &b) {
            return std::get<0>(a) > std::get<0>(b);
        }
    };

    using IndexT = unsigned long long;

    template<typename T, typename QueueType = std::queue<T>>
    class SimpleQueue {
      public:
        SimpleQueue() = default;
        SimpleQueue(const SimpleQueue &) = delete;// disable copying
        SimpleQueue &operator=(const SimpleQueue &) = delete;// disable assignment
        T pop();
        size_t size();
        void pop(T &item);
        bool isOpen() const;
        // Call `invalidate` when the producer has finished producing data and it is terminating.
        // The consumers will have to call the function `valid` to check if the stream is still
        // valid
        void close();
        template<typename U = T, typename Q = QueueType>
        typename std::enable_if<std::is_same<Q, std::priority_queue<U>>::value, U>::type pop_index(
            IndexT idx);
        T front();
        void front(T &item);
        void push(const T &item);

      private:
        QueueType queue_;
        mutable std::mutex mutex_;
        std::condition_variable cond_;
        bool isOpenCache_ = true;
        bool isOpen_ = true;
    };
}// namespace Concurrency
}// namespace rtb

#include "SimpleQueue.cpp"
#endif