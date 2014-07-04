#ifndef Condition_h
#define Condition_h

#include <mutex>

namespace CEINMS { namespace Concurrency {

    class Condition {
    private:
        bool value_;
        std::mutex mutex_;
    public:
        Condition(bool value) : value_(value) {}
        void set(bool value) {
            mutex_.lock(); value_ = value; mutex_.unlock();
        }
        bool get() { return value_; }
    };
	
} } // namespace

#endif
