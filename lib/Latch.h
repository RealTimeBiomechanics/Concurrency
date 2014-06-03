#ifndef Latch_h
#define Latch_h

#include <thread>
#include <mutex>
#include <condition_variable>

class Latch {
public:
  Latch(int count);

  void arriveAndWait();

  Latch(const Latch&) = delete;
  Latch& operator=(const Latch&) = delete; 
private:

  int count_;

  std::condition_variable condition_;
  std::mutex mutex_;

};

#include "Latch.cpp"

#endif  