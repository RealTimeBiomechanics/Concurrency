#ifndef Latch_h
#define Latch_h

#include <thread>
#include <mutex>
#include <condition_variable>

namespace CEINMS{ namespace Concurrency{
class Latch {
public:
  Latch();
  Latch(int count);

  void setCount(int count);

  void wait();


  Latch(const Latch&) = delete;
  Latch& operator=(const Latch&) = delete;
private:

  int count_;

  std::condition_variable condition_;
  std::mutex mutex_;

};

}; };

#endif
