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
#include <optional>

namespace rtb {
namespace Concurrency {
    /// SimpleQueue is an implementation of a producer consumer pattern
    /** Each message is removed from SimpleQueue after being read by any of the consumers. */
    template<typename T>
    using IndexedData = std::tuple<unsigned long long, T>;

    template<typename T>
    struct OrderByIndex {
    bool operator()(T const &a, T const &b) {
            if (!a.has_value()) return 1;
            if (!b.has_value()) return 0;
            return std::get<0>(a.value()) > std::get<0>(b.value());
        }

    };

    using IndexT = unsigned long long;

    template<typename T, typename QueueType = std::queue<std::optional<T>>>
    class SimpleQueue;

    using IndexQueue = SimpleQueue<IndexT>;

    template<typename T>
    using IndexedDataQueue = SimpleQueue<IndexedData<T>>;

    template<typename T>
    using PriorityQueue = std::priority_queue<std::optional<T>, std::vector<std::optional<T>>, OrderByIndex<std::optional<T>>>;

    template<typename T>
    using IndexedPriorityQueue = std::priority_queue < std::optional<IndexedData<T>>,
        std::vector<std::optional<IndexedData<T>>>,
        OrderByIndex<std::optional<IndexedData<T>>>>;


    template<typename T>
    using SortedIndexedDataQueue = SimpleQueue<IndexedData<T>, IndexedPriorityQueue<T>>;


    template<typename T, typename QueueType>
    class SimpleQueue {
      public:

        SimpleQueue() = default;
        SimpleQueue(const SimpleQueue &) = delete;// disable copying
        SimpleQueue &operator=(const SimpleQueue &) = delete;// disable assignment
        std::optional<T> pop();
        size_t size();
        void close();
        template<typename U = T, typename Q = QueueType>
        typename std::enable_if<std::is_same<Q, PriorityQueue<U>>::value,
            std::optional<T>>::type
            popIndex(IndexT idx);
        std::optional<T> front();
        void push(const T &item);

      private:
        void push(const std::optional<T> &item);
        QueueType queue_;
        mutable std::mutex mutex_;
        std::condition_variable cond_;
    };
}// namespace Concurrency
}// namespace rtb

#include "SimpleQueue.cpp"
#endif