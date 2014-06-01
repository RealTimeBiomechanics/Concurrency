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


#include "Queue.h"
#include <iostream>
#include <boost/iterator/iterator_concepts.hpp>

  template <typename T>
  T Queue<T>::pop() {
    
    std::unique_lock<std::mutex> mlock(mutex_);
    while (subscribersMissingRead_[std::this_thread::get_id()] == 0) {
      cond_.wait(mlock);
    }
    
    T val = *subscribersNextRead_[std::this_thread::get_id()];

    // advance iterator (maybe goes to .end()) 
    subscribersNextRead_[std::this_thread::get_id()]++;
    subscribersMissingRead_[std::this_thread::get_id()]--; 
    
    if (!someoneSlowerThanMe()) { 
      queue_.pop_front();  
    }
    
    mlock.unlock();
    return val;
    
  }
    
  // push data only when the queue has subscribers 
  template <typename T>  
  void Queue<T>::push(const T& item) {
    
    std::unique_lock<std::mutex> mlock(mutex_);
    if (!subscribersNextRead_.empty()) 
      queue_.push_back(item);

    // if you had nothing to read...now you have something
    for (auto& it : subscribersNextRead_) {
      if (subscribersMissingRead_[it.first] == 0)
	it.second = (++queue_.rbegin()).base();
    }
    // new message to be read by everyone
    for (auto& it : subscribersMissingRead_) {
      it.second +=1; 
    }
     
    mlock.unlock();
    
    // maybe no subscribers but do anyway
    cond_.notify_all();
    
  }
    
  template <typename T>
  void Queue<T>::subscribe() {
    std::unique_lock<std::mutex> mlock(mutex_);
    if ( queue_.empty() ) {
      subscribersNextRead_[std::this_thread::get_id()] = queue_.end();
      subscribersMissingRead_[std::this_thread::get_id()] = 0;
    }
    else {
      subscribersNextRead_[std::this_thread::get_id()] = (++queue_.rbegin()).base();
      subscribersMissingRead_[std::this_thread::get_id()] = 1;
    }
    mlock.unlock();
  }
  
  template <typename T>
  void Queue<T>::unsubscribe() {
    std::unique_lock<std::mutex> mlock(mutex_);
    
    int myMsgToRead = 0; 
    if (!someoneSlowerThanMe()) {
       int myMsgToRead = std::max_element(subscribersMissingRead_.begin(), subscribersMissingRead_.end(), pred) -> second;
       subscribersMissingRead_.erase(std::this_thread::get_id());
       int otherMaxMsgToRead = 0; 
       if (subscribersMissingRead_.size() != 0) 
         otherMaxMsgToRead = std::max_element(subscribersMissingRead_.begin(), subscribersMissingRead_.end(), pred) -> second;
       for (int i=0; i < (otherMaxMsgToRead - myMsgToRead); ++i)
	 queue_.pop_front();
    } 
    else 
        subscribersMissingRead_.erase(std::this_thread::get_id());
    
    subscribersNextRead_.erase(std::this_thread::get_id()); 
    
    mlock.unlock();
  }
  
  template <typename T>
  bool Queue<T>::pred(const std::pair< std::thread::id, int>& lhs, const std::pair< std::thread::id, int>& rhs) {
    return lhs.second < rhs.second;
  }
 

  template <typename T>
  bool Queue<T>::someoneSlowerThanMe() {
    
    int maxNoMsgToRead = std::max_element(subscribersMissingRead_.begin(), subscribersMissingRead_.end(), pred) -> second;
    
    if (maxNoMsgToRead > subscribersMissingRead_[std::this_thread::get_id()])
      return true;
    
    return false;
    
  }
  
  template <typename T>
  std::ostream& operator<<(std::ostream& os,  Queue<T>& queue) {
    
    std::unique_lock<std::mutex> mlock(queue.mutex_);
    std::cout << "Messages in the queue: \n";
    for(auto& it : queue.queue_) 
      std::cout << it << " ";
    std::cout << std::endl;
     
    std::cout << "Next messages to be read: \n";
    for(auto& it : queue.subscribersNextRead_) 
      std::cout << it.first << " " << *(it.second) << "\n";
    std::cout << std::endl;
    
    std::cout << "Number of messages to be read: \n"; 
    for(auto& it : queue.subscribersMissingRead_)
      std::cout << it.first << " " << it.second << "\n";
    std::cout << std::endl;
    
    mlock.unlock();
    return os;
}
  

