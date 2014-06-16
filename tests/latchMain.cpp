#include <iostream>
#include <vector>
#include <chrono>
#include "Latch.h"

std::mutex mutexOutput; 

typedef std::chrono::milliseconds TimeT; 

// all times in milliseconds
void doingNothing(Latch& l, int i, TimeT sleepingBeforeLatching) {
  auto start = std::chrono::system_clock::now(); 
  std::this_thread::sleep_for(sleepingBeforeLatching);
  
  auto stop = std::chrono::system_clock::now(); 
  std::unique_lock<std::mutex> lock1(mutexOutput);
  std::cout << "thread " << i << " entering  the barrier at: " << (stop-start).count() << std::endl; 
  lock1.unlock();
  
  l.wait();
  
  stop = std::chrono::system_clock::now(); 
  std::unique_lock<std::mutex> lock2(mutexOutput);
  std::cout << "thread " << i << " went through the barrier at: " << (stop-start).count() << std::endl; 
  lock2.unlock();
}


int main()
{
  Latch l(5);
  
  
  std::thread thread1(std::bind(doingNothing, std::ref(l), 1, TimeT{1000}));
  std::thread thread2(std::bind(doingNothing, std::ref(l), 2, TimeT{2000}));
  std::thread thread3(std::bind(doingNothing, std::ref(l), 3, TimeT{3000}));
  std::thread thread4(std::bind(doingNothing, std::ref(l), 4, TimeT{4000}));
  std::thread thread5(std::bind(doingNothing, std::ref(l), 5, TimeT{5000}));  

 
  thread1.join(); 
  thread2.join(); 
  thread3.join(); 
  thread4.join(); 
  thread5.join(); 
  
  Latch l2(4);

  l2.setCount(5);
  
  
  std::thread thread11(std::bind(doingNothing, std::ref(l2), 1, TimeT{1000}));
  std::thread thread12(std::bind(doingNothing, std::ref(l2), 2, TimeT{2000}));
  std::thread thread13(std::bind(doingNothing, std::ref(l2), 3, TimeT{3000}));
  std::thread thread14(std::bind(doingNothing, std::ref(l2), 4, TimeT{4000}));
  std::thread thread15(std::bind(doingNothing, std::ref(l2), 5, TimeT{5000}));  

 
  thread11.join(); 
  thread12.join(); 
  thread13.join(); 
  thread14.join(); 
  thread15.join(); 
  
  
  
}