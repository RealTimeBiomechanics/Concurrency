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
#include <algorithm>

namespace rtb {
namespace Concurrency {

    template<typename T>
    std::optional<T> Queue<T>::pop() {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (subscribersMissingRead_[std::this_thread::get_id()] == 0) {
            cond_.wait(mlock);
        }
        auto val{ *subscribersNextRead_[std::this_thread::get_id()] };
        // advance iterator (maybe goes to .end())
        subscribersNextRead_[std::this_thread::get_id()]++;
        subscribersMissingRead_[std::this_thread::get_id()]--;

        if (!someoneSlowerThanMe()) { queue_.pop_front(); }

        mlock.unlock();
        return val;
    }

    // push data only when the queue has subscribers
    template<typename T>
    void Queue<T>::push(const T &item) {
        push(std::optional<T>{ item });
    }

    template<typename T>
    void Queue<T>::close() {
        push(std::optional<T>{});
    }

    template<typename T>
    void Queue<T>::push(const std::optional<T> &item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        if (!subscribersNextRead_.empty()) queue_.push_back(std::optional<T>{ item });

        // if you had nothing to read...now you have something
        for (auto &it : subscribersNextRead_) {
            if (subscribersMissingRead_[it.first] == 0) it.second = (++queue_.rbegin()).base();
        }
        // new message to be read by everyone
        for (auto &it : subscribersMissingRead_) {
            it.second += 1;
        }
        mlock.unlock();

        // maybe no subscribers but do anyway
        cond_.notify_all();
    }

    template<typename T>
    size_t Queue<T>::messagesToRead() const {
        std::lock_guard<std::mutex> guard{ mutex_ };
        return subscribersMissingRead_.at(std::this_thread::get_id());
    }

    template<typename T>
    void Queue<T>::subscribe() {
        std::unique_lock<std::mutex> mlock(mutex_);
        if (queue_.empty()) {
            subscribersNextRead_[std::this_thread::get_id()] = queue_.end();
            subscribersMissingRead_[std::this_thread::get_id()] = 0;
        } else {
            subscribersNextRead_[std::this_thread::get_id()] = (++queue_.rbegin()).base();
            subscribersMissingRead_[std::this_thread::get_id()] = 1;
        }
        mlock.unlock();
    }

    template<typename T>
    void Queue<T>::unsubscribe() {
        std::unique_lock<std::mutex> mlock(mutex_);

        int myMsgToRead = 0;
        if (!someoneSlowerThanMe()) {
            int myMsgToRead = std::max_element(
                subscribersMissingRead_.begin(), subscribersMissingRead_.end(), pred)
                                  ->second;
            subscribersMissingRead_.erase(std::this_thread::get_id());
            int otherMaxMsgToRead = 0;
            if (subscribersMissingRead_.size() != 0)
                otherMaxMsgToRead = std::max_element(
                    subscribersMissingRead_.begin(), subscribersMissingRead_.end(), pred)
                                        ->second;
            for (int i = 0; i < (otherMaxMsgToRead - myMsgToRead); ++i)
                queue_.pop_front();
        } else
            subscribersMissingRead_.erase(std::this_thread::get_id());

        subscribersNextRead_.erase(std::this_thread::get_id());

        mlock.unlock();
    }

    template<typename T>
    bool Queue<T>::pred(const std::pair<std::thread::id, int> &lhs,
        const std::pair<std::thread::id, int> &rhs) {
        return lhs.second < rhs.second;
    }

    template<typename T>
    bool Queue<T>::someoneSlowerThanMe() {
        int maxNoMsgToRead =
            std::max_element(subscribersMissingRead_.begin(), subscribersMissingRead_.end(), pred)
                ->second;

        if (maxNoMsgToRead > subscribersMissingRead_[std::this_thread::get_id()]) return true;

        return false;
    }

}// namespace Concurrency
}// namespace rtb
