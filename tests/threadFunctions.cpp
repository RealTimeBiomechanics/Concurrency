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

#include "threadFunctions.h"
#include "Queue.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>





// all times in milliseconds
void produce(Queue<int>& q, TimeT startTime, TimeT period, int noMessages) {
  auto start = std::chrono::system_clock::now(); 
  std::this_thread::sleep_for(startTime);
  
  for (int i = 1; i <= noMessages; ++i) {
    q.push(i);
    
    //std::cout << q << std::endl;
    std::this_thread::sleep_for(period);
  }
  q.push( std::numeric_limits<int>::max() );
  //std::cout << q << std::endl;
}

void consume(Queue<int>& q, const int id,  const std::vector<TimeT>& subscribeTime, const std::vector<TimeT>& unsubscribeTime, TimeT readingCycleTime ) {
 
  std::vector<int> consumedValues;
  bool lastMessage = false;
  unsigned nextConsumerPeriod = 0;
  
  auto start = std::chrono::system_clock::now(); 
  
  std::this_thread::sleep_for( subscribeTime.at(nextConsumerPeriod) );
  
  //:FIXME: start time for consumers and producer are not exactly the same... 
  // but for test purposes is ok
     
  do {
    q.subscribe();
    
    bool timeToUnsubscribe = false; 
    
    do {
      std::this_thread::sleep_for( readingCycleTime ); 
      auto item = q.pop();
      lastMessage = (item == std::numeric_limits<int>::max());
      if (!lastMessage)
        consumedValues.push_back(item);
      
      timeToUnsubscribe = ( unsubscribeTime.at(nextConsumerPeriod) <= 
                             std::chrono::duration_cast< TimeT >( std::chrono::system_clock::now() - start ) ) ;      


    } while ( !lastMessage && !timeToUnsubscribe);
   // std::cout << "BEFORE:\n" << q << std::endl;
    q.unsubscribe(); 
    //  std::cout << "AFTER:\n" << q << std::endl;
    nextConsumerPeriod++; 
    if (nextConsumerPeriod < subscribeTime.size() ) {
      std::this_thread::sleep_for( subscribeTime.at(nextConsumerPeriod) - unsubscribeTime.at(nextConsumerPeriod-1));	
    }
  } while ( (!lastMessage) && (nextConsumerPeriod < subscribeTime.size() ) );

  
  std::unique_lock<std::mutex> mlock(outputMutex);
  std::cout << "Consumer " << id << " (" << std::this_thread::get_id() << ")  popped the following values: \n"; 
  for (auto& it : consumedValues )
    std::cout << it << " ";
  std::cout << std::endl; 
  mlock.unlock();

}
