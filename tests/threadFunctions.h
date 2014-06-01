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
#include <thread>
#include <chrono>  

extern std::mutex outputMutex;

typedef std::chrono::milliseconds TimeT; 
// all times in milliseconds
void produce(Queue<int>& q, TimeT startTime, TimeT period, int noMessages);

void consume(Queue<int>& q, const int id,  const std::vector<TimeT>& subscribeTime, const std::vector<TimeT>& unsubscribeTime, TimeT readingCycleTime );