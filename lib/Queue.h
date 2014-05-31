//   Queue - an implementation of a single producer multiple consumers 
//           with the following constraints:
//           - the consumers can register/deregister to the queue at run time
//           - all the messages MUST be consumed by all the consumers
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
    
    T pop();
    void push(const T& item);
  
    void subscribe(); 
    void unsubscribe();
    
    Queue() = default;
    Queue(const Queue&) = delete;            // disable copying
    Queue& operator=(const Queue&) = delete; // disable assignment
   
    template <typename B>
    friend std::ostream& operator<< (std::ostream& os,  Queue<B>& queue);
  
  private:
    
    //:TODO: move to deque if you go with a circular list (list has time 1 of access.. deque is more like a vector!!
    // BUT WATCH OUT... NEVER EVER USE something that can be reallocated... otherwise bye bye to the iterator and pointers
    std::list<T> queue_;
    typedef typename std::list<T>::iterator QueueIterator; 
    std::map< std::thread::id, QueueIterator > subscribersNextRead_; 
    std::map< std::thread::id, int > subscribersMissingRead_;
    std::mutex mutex_;
    std::condition_variable cond_;
    
    bool someoneSlowerThanMe();
};

#include "Queue.cpp"

#endif
     //movable semantic  bool push (T &&item /*, ...*/)).

    // is it possible to use emplace?

  
    