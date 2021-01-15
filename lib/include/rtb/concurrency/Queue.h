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
#ifndef rtb_Queue_h
#define rtb_Queue_h

#include <list>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>

namespace rtb {
namespace Concurrency {
    //   Queue - an implementation of a single producer multiple consumers design pattern
    //           with the following constraints:
    //           - the consumers can subscribe/unsubscribe to the queue at run time
    //           - all the messages MUST be consumed by all the subscribed consumers
    template<typename T>
    class Queue {
      public:
        typedef T type;
        Queue() = default;
        Queue(const Queue &) = delete;
        Queue &operator=(const Queue &) = delete;
        void subscribe();
        void unsubscribe();
        // returns no value when the queue has been closed
        std::optional<T> pop();
        void push(const T &item);
        size_t messagesToRead() const;
        // Call `close` when the producer has finished producing data and it is terminating.
        void close();

      private:
 
        // decided to go with a list so we can trust the iterator. With other containers you can
        // have reallocation that invalidates iterator
        std::list<std::optional<T>> queue_;
        typedef typename std::list<std::optional<T>>::iterator QueueIterator;
        // could be a single map with a structure. But keep in this way cause it helps in function
        // unsubscribe
        std::map<std::thread::id, QueueIterator> subscribersNextRead_;
        std::map<std::thread::id, int> subscribersMissingRead_;
        mutable std::mutex mutex_;
        std::condition_variable cond_;
        // utility function used to find the maximum on a map
        void push(const std::optional<T> &item);
        static bool pred(const std::pair<std::thread::id, int> &lhs,
            const std::pair<std::thread::id, int> &rhs);
        bool someoneSlowerThanMe();
    };
}// namespace Concurrency
}// namespace rtb

#include "Queue.cpp"
#endif
