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

namespace rtb {
namespace Concurrency {

    template<typename T, typename QueueType>
    std::optional<T> SimpleQueue<T, QueueType>::pop() {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty()) {
            cond_.wait(mlock);
        }
        auto val{ queue_.front() };
        queue_.pop();
        mlock.unlock();
        return val;
    }

        template<typename T, typename QueueType>
    std::optional<T> SimpleQueue<T, QueueType>::front() {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty() && isOpen_) {
            cond_.wait(mlock);
        }
        auto val{ queue_.front() };
        mlock.unlock();
        return val;
    }


    template<typename T, typename QueueType>
    size_t SimpleQueue<T, QueueType>::size() {
        std::lock_guard<std::mutex> mlock(mutex_);
        return queue_.size();
    }

   
    template<typename T, typename QueueType>
    template<typename U, typename Q>
    typename std::enable_if<std::is_same<Q, PriorityQueue<U>>::value,
        std::optional<T>>::type
        SimpleQueue<T, QueueType>::popIndex(IndexT idx) {
        std::unique_lock<std::mutex> mlock(mutex_);
     
        while (queue_.empty() || std::get<0>(queue_.top().value_or(std::make_tuple<unsigned long long, double>(99,0.)))
                    != idx) {
            cond_.wait(mlock);
        }
        std::optional<T> val{ queue_.top() };
        queue_.pop();
        mlock.unlock();
        return val;
    }


    template<typename T, typename QueueType>
    void SimpleQueue<T, QueueType>::push(const T &item) {
        push(std::optional<T>{ item });
    }

    template<typename T, typename QueueType>
    void SimpleQueue<T, QueueType>::push(const std::optional<T>& item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(item);
        mlock.unlock();
        cond_.notify_one();
    }

    template<typename T, typename QueueType>
    void SimpleQueue<T, QueueType>::close() {
        push(std::optional<T>{});
    }


}// namespace Concurrency
}// namespace rtb
