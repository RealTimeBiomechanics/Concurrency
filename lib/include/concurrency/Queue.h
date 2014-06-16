//   Queue - an implementation of a single producer multiple consumers
//           with the following constraints:
//           - the consumers can subscribe/unsubscribe to the queue at run time
//           - all the messages MUST be consumed by all the subscribed consumers
//
//   Copyright (C) 2014 Monica Reggiani <monica.reggiani@gmail.com>
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef Queue_h
#define Queue_h


#include <list>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class Queue {
  public:

    Queue() = default;
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    void subscribe();
    void unsubscribe();

    T pop();
    void push(const T& item);

    template <typename B>
    friend std::ostream& operator<< (std::ostream& os,  const Queue<B>& queue);

  private:

    // decided to go with a list so we can trust the iterator. With other containers you can have
    // reallocation that invalidates iterator
    std::list<T> queue_;
    typedef typename std::list<T>::iterator QueueIterator;

    // could be a single map with a structure. But keep in this way cause it helps in function unsubscribe
    std::map< std::thread::id, QueueIterator > subscribersNextRead_;
    std::map< std::thread::id, int > subscribersMissingRead_;

    std::mutex mutex_;
    std::condition_variable cond_;

    // utility function used to find the maximum on a map
    static bool pred(const std::pair< std::thread::id, int>& lhs, const std::pair< std::thread::id, int>& rhs);
    bool someoneSlowerThanMe();
};

#include "Queue.cpp"

#endif



