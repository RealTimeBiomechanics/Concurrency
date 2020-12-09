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
        T pop();
        void push(const T &item);
        template<typename B>
        friend std::ostream &operator<<(std::ostream &os, const Queue<B> &queue);

      private:
        // decided to go with a list so we can trust the iterator. With other containers you can
        // have reallocation that invalidates iterator CP: c++11 should have introduced std::next
        // and std::prev for iterators..
        std::list<T> queue_;
        typedef typename std::list<T>::iterator QueueIterator;
        // could be a single map with a structure. But keep in this way cause it helps in function
        // unsubscribe
        std::map<std::thread::id, QueueIterator> subscribersNextRead_;
        std::map<std::thread::id, int> subscribersMissingRead_;
        std::mutex mutex_;
        std::condition_variable cond_;

        // utility function used to find the maximum on a map
        static bool pred(const std::pair<std::thread::id, int> &lhs,
            const std::pair<std::thread::id, int> &rhs);
        bool someoneSlowerThanMe();
    };
}// namespace Concurrency
}// namespace rtb

#include "Queue.cpp"
#endif
