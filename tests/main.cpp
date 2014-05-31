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

#include "Queue.h"
#include <iostream>
#include <thread>
#include <chrono> 


std::mutex outputMutex;
   
void produce(Queue<int>& q) {
  std::this_thread::sleep_for(std::chrono::milliseconds{50});
  for (int i = 0; i< 100; ++i) {
    q.push(i);
  }
}

void consume(Queue<int>& q, unsigned int id) {
  
  std::vector<int> consumedValues; 
 
  std::this_thread::sleep_for(std::chrono::milliseconds{30});
  q.subscribe();
  for (int i = 0; i< 100; ++i) {
     auto item = q.pop();
     consumedValues.push_back(item);
  }
  q.unsubscribe(); 
  
  outputMutex.lock();
  std::cout << "Consumer " << id << " popped the following values: \n"; 
  for (auto& it : consumedValues )
    std::cout << it << " ";
  outputMutex.unlock();
  std::cout << std::endl; 

}

int main()
{
  Queue<int> q;

  using namespace std::placeholders;

  // producer thread
  std::thread prod1(std::bind(produce, std::ref(q)));

  // consumer threads
  std::thread consumer1(std::bind(&consume, std::ref(q), 1));
  std::thread consumer2(std::bind(&consume, std::ref(q), 2));
  std::thread consumer3(std::bind(&consume, std::ref(q), 3));
  std::thread consumer4(std::bind(&consume, std::ref(q), 4));

  prod1.join();
  consumer1.join();
  consumer2.join();
  consumer3.join();
  consumer4.join();

}