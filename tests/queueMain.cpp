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
#include <iostream>
#include <vector>

using namespace CEINMS::Concurrency;

int main()
{

    
  // FIRST TEST
  // Producer start first (at 10) and send 100 msg with a period of 10
  // 4 Consumers subscribe before (at 5) and unsubscribe at 1500
  // OUTPUT: all the consumers should read everything
  
  std::cout << "\n ---------------- First Test ---------------- \n";
  std::cout << "OUTPUT:  all the consumers read 100 messages\n\n"; 
  
  std::vector<TimeT> subscribeTimeTest1 = {TimeT{5}}; 
  std::vector<TimeT> unsubscribeTimeTest1 = {TimeT{1500}};
  
  Queue<int> q1;
  
  // producer thread
  std::thread prodTest1(std::bind(produce, std::ref(q1), TimeT{10}, TimeT{10}, 100));

  // consumer threads
  std::thread consumer1Test1(std::bind(&consume, std::ref(q1), 1, std::ref(subscribeTimeTest1), std::ref(unsubscribeTimeTest1), TimeT{0}  ));
  std::thread consumer2Test1(std::bind(&consume, std::ref(q1), 2, std::ref(subscribeTimeTest1), std::ref(unsubscribeTimeTest1), TimeT{0}  ));
  std::thread consumer3Test1(std::bind(&consume, std::ref(q1), 3, std::ref(subscribeTimeTest1), std::ref(unsubscribeTimeTest1), TimeT{0}  ));
  std::thread consumer4Test1(std::bind(&consume, std::ref(q1), 4, std::ref(subscribeTimeTest1), std::ref(unsubscribeTimeTest1), TimeT{0}  ));

  prodTest1.join();
  consumer1Test1.join();
  consumer2Test1.join(); 
  consumer3Test1.join(); 
  consumer4Test1.join(); 
  
  
  std::cout << "\nFinal state of the queue: \n";
  std::cout << q1 << "\n\n"; 
  
  // SECOND TEST
  // Producer start first (at 10) and send 100 msg with a period of 10
  // Consumer 1 and 2 subscribe before (at 5), 
  // Consumer 3 and 4 subscribe after (at 500) and read half of the messages
  // Consumer 5 and 6 subscribe before (at 5) andn leave in the middle 
  // OUTPUT: Consumers 1 and 2 100 messages
  //         Consumers 3 and 4 roughly the last half of the messages
  //         Consumers 5 and 6 roughly the first half of the messages
  
  
  std::cout << "\n ---------------- Second Test ----------------\n";
  std::cout << "OUTPUT: Consumers 1 and 2 read messages from 1 to 100\n"; 
  std::cout << "        Consumers 3 and 4 approximatively from 50 to 100\n";
  std::cout << "        Consumers 5 and 6 approximatively from 1 to 50\n\n";
  
  
  Queue<int> q2;
  
  std::vector<TimeT> subscribeTimeTest2Consumer1and2 = {TimeT{5}}; 
  std::vector<TimeT> unsubscribeTimeTest2Consumer1and2 = {TimeT{1500}};
  std::vector<TimeT> subscribeTimeTest2Consumer3and4 = {TimeT{500}}; 
  std::vector<TimeT> unsubscribeTimeTest2Consumer3and4 = {TimeT{1500}};
  std::vector<TimeT> subscribeTimeTest2Consumer5and6 = {TimeT{5}}; 
  std::vector<TimeT> unsubscribeTimeTest2Consumer5and6 = {TimeT{510}};
  
  // producer thread
  std::thread prodTest2(std::bind(produce, std::ref(q2), TimeT{10}, TimeT{10}, 100));

  // consumer threads
  std::thread consumer1Test2(std::bind(&consume, std::ref(q2), 1, std::ref(subscribeTimeTest2Consumer1and2), std::ref(unsubscribeTimeTest2Consumer1and2), TimeT{0}  ));
  std::thread consumer2Test2(std::bind(&consume, std::ref(q2), 2, std::ref(subscribeTimeTest2Consumer1and2), std::ref(unsubscribeTimeTest2Consumer1and2), TimeT{0}  ));
  std::thread consumer3Test2(std::bind(&consume, std::ref(q2), 3, std::ref(subscribeTimeTest2Consumer3and4), std::ref(unsubscribeTimeTest2Consumer3and4), TimeT{0}  ));
  std::thread consumer4Test2(std::bind(&consume, std::ref(q2), 4, std::ref(subscribeTimeTest2Consumer3and4), std::ref(unsubscribeTimeTest2Consumer3and4), TimeT{0}  ));
  std::thread consumer5Test2(std::bind(&consume, std::ref(q2), 5, std::ref(subscribeTimeTest2Consumer5and6), std::ref(unsubscribeTimeTest2Consumer5and6), TimeT{0}  ));
  std::thread consumer6Test2(std::bind(&consume, std::ref(q2), 6, std::ref(subscribeTimeTest2Consumer5and6), std::ref(unsubscribeTimeTest2Consumer5and6), TimeT{0}  ));

  prodTest2.join();
  consumer1Test2.join();
  consumer2Test2.join(); 
  consumer3Test2.join(); 
  consumer4Test2.join(); 
  consumer5Test2.join(); 
  consumer6Test2.join(); 
  
  std::cout << "\nFinal state of the queue: \n";
  std::cout << q2 << "\n\n"; 
  
  
  // THIRD TEST
  // Producer start first (at 10) and send 100 msg with a period of 10  
  // OUTPUT: Consumers 1 and 2  1 to 25 and 50 to 75
  //         Consumers 3 and 4  25 to 50 and  75 to 100
  
  std::cout << "\n ---------------- Third Test ----------------\n";
  std::cout << "OUTPUT: Consumers 1 and 2 aapproximatively  from 1 to 25 and 50 to 75\n"; 
  std::cout << "        Consumers 3 and 4 approximatively from 25 to 50 and 75 to 100\n\n";
  
  
  Queue<int> q3;
  
  std::vector<TimeT> subscribeTimeTest3Consumer1and2 = {TimeT{5}, TimeT{500}}; 
  std::vector<TimeT> unsubscribeTimeTest3Consumer1and2 = {TimeT{250}, TimeT{760}};
  
  std::vector<TimeT> subscribeTimeTest3Consumer3and4 = {TimeT{250}, TimeT{750}}; 
  std::vector<TimeT> unsubscribeTimeTest3Consumer3and4 = {TimeT{500}, TimeT{1010}};
  
  
  // producer thread
  std::thread prodTest3(std::bind(produce, std::ref(q3), TimeT{10}, TimeT{10}, 100));

  // consumer threads
  std::thread consumer1Test3(std::bind(&consume, std::ref(q3), 1, std::ref(subscribeTimeTest3Consumer1and2), std::ref(unsubscribeTimeTest3Consumer1and2), TimeT{0} ));
  std::thread consumer2Test3(std::bind(&consume, std::ref(q3), 2, std::ref(subscribeTimeTest3Consumer1and2), std::ref(unsubscribeTimeTest3Consumer1and2), TimeT{0} ));
  std::thread consumer3Test3(std::bind(&consume, std::ref(q3), 3, std::ref(subscribeTimeTest3Consumer3and4), std::ref(unsubscribeTimeTest3Consumer3and4), TimeT{0} ));
  std::thread consumer4Test3(std::bind(&consume, std::ref(q3), 4, std::ref(subscribeTimeTest3Consumer3and4), std::ref(unsubscribeTimeTest3Consumer3and4), TimeT{0} ));
  
  prodTest3.join();
  consumer1Test3.join();
  consumer2Test3.join(); 
  consumer3Test3.join(); 
  consumer4Test3.join(); 

  std::cout << "\nFinal state of the queue: \n";
  std::cout << q3 << "\n\n"; 
  
  
  // FOURTH TEST
  // This test check if unread messages from the leaving client (the slowest one) are removed from the queue
  // Producer start first (at 10) and send 100 msg with a period of 10
  // Three consumers
  // First one goes on for the whole time reading all the messages
  // Second one (slowest) stop at 500  and remove all the messages still to be read
  // The third one, arrive later (at 700) and should start reading aroung message 70
  
  std::cout << "\n ---------------- Fourth Test ---------------- \n";
  std::cout << "OUTPUT: Consumers 1 read all the 100 messages\n";
  std::cout << "        Consumers 2 (the slowest one) stop at 500 but should read half of the available\n";
  std::cout << "        Consumers 3 approximatively  from message 70\n\n";
  
  
  Queue<int> q4;
  
  std::vector<TimeT> subscribeTimeTest4Consumer1 = {TimeT{5}}; 
  std::vector<TimeT> unsubscribeTimeTest4Consumer1 = {TimeT{1010}};
  
  std::vector<TimeT> subscribeTimeTest4Consumer2 = {TimeT{5}}; 
  std::vector<TimeT> unsubscribeTimeTest4Consumer2 = {TimeT{500}};
  
  std::vector<TimeT> subscribeTimeTest4Consumer3 = {TimeT{700}}; 
  std::vector<TimeT> unsubscribeTimeTest4Consumer3 = {TimeT{1010}};
  
  // producer thread
  std::thread prodTest4(std::bind(produce, std::ref(q3), TimeT{10}, TimeT{10}, 100));

  // consumer threads
  std::thread consumer1Test4(std::bind(&consume, std::ref(q3), 1, std::ref(subscribeTimeTest4Consumer1), std::ref(unsubscribeTimeTest4Consumer1), TimeT{0}  ));
  std::thread consumer2Test4(std::bind(&consume, std::ref(q3), 2, std::ref(subscribeTimeTest4Consumer2), std::ref(unsubscribeTimeTest4Consumer2), TimeT{20}));
  std::thread consumer3Test4(std::bind(&consume, std::ref(q3), 3, std::ref(subscribeTimeTest4Consumer3), std::ref(unsubscribeTimeTest4Consumer3), TimeT{0}  )); 
  
  prodTest4.join();
  consumer1Test4.join();
  consumer2Test4.join(); 
  consumer3Test4.join(); 
  
  std::cout << "\nFinal state of the queue: \n";
  std::cout << q4 << "\n\n"; 
  
  
  // FIFTH TEST
  // Producer start first (at 10) and send 100 msg with a period of 10
  // Consumers start after the last msg has been sent 
  // OUTPUT: Consumers block 
  
  std::cout << "\n ---------------- Last Test ---------------- \n";
  std::cout << " Consumers start after the producer has left\n"; 
  std::cout << " OUTPUT: They blocks trying to pop... they are always waiting for a msg \n\n"; 
  
  
  Queue<int> q5;
  
  std::vector<TimeT> subscribeTimeTest5 = {TimeT{1500}}; 
  std::vector<TimeT> unsubscribeTimeTest5 = {TimeT{3000}};
  
  // producer thread
  std::thread prodTest5(std::bind(produce, std::ref(q5), TimeT{10}, TimeT{10}, 100));

  // consumer threads
  std::thread consumer1Test5(std::bind(&consume, std::ref(q5), 1, std::ref(subscribeTimeTest5), std::ref(unsubscribeTimeTest5), TimeT{0}  ));
  std::thread consumer2Test5(std::bind(&consume, std::ref(q5), 2, std::ref(subscribeTimeTest5), std::ref(unsubscribeTimeTest5), TimeT{0}  ));
  std::thread consumer3Test5(std::bind(&consume, std::ref(q5), 3, std::ref(subscribeTimeTest5), std::ref(unsubscribeTimeTest5), TimeT{0}  ));
  std::thread consumer4Test5(std::bind(&consume, std::ref(q5), 4, std::ref(subscribeTimeTest5), std::ref(unsubscribeTimeTest5), TimeT{0}  ));

  prodTest5.join();
  consumer1Test5.join();
  consumer2Test5.join(); 
  consumer3Test5.join(); 
  consumer4Test5.join(); 
  
  std::cout << "\nFinal state of the queue: \n";
  std::cout << q5 << "\n\n"; 
  

}
