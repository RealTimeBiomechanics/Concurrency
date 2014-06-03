#include "Latch.h"
#include <iostream>
#include <algorithm>

Latch::Latch(int count)
    : count_(count) {
}


void Latch::arriveAndWait() {
    std::unique_lock<std::mutex> mlock(mutex_);
    if (count_ == 0) {
      throw std::logic_error("internal count == 0");
    }
    if (--count_ == 0) 
      condition_.notify_all();
    else {
      while(count_ > 0) 
        condition_.wait(mlock);
    }
    mlock.unlock();
}

