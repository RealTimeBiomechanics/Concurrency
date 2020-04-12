#include "rtb/concurrency/Queue.h"
#include "rtb/concurrency/Latch.h"
#include "rtb/concurrency/ThreadPool.h"
#include <chrono>
#include <iostream>
#include <type_traits>

using namespace rtb::Concurrency;
struct AddOne {
	using InputData = int;
	using OutputData = int;
	int operator()(int value) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		return value + 1;
	}
};


struct Source {
	Source(Queue<int>& outputQueue) :
		outputQueue_(outputQueue) {}
	void operator()() {
		int val = 0;
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			outputQueue_.push(val++);
		}
	}

private:
	Queue<int>& outputQueue_;
};


struct Sink {
	Sink(Queue<int>& inputQueue) :
		inputQueue_(inputQueue) {}
	void operator()() {
		inputQueue_.subscribe();
		while (true) {
			std::cout << inputQueue_.pop() << std::endl;
		}
	}
private:
	Queue<int>& inputQueue_;
};
/*

class foo;
class bar;

template<class T>
struct is_bar
{
	template<class Q = T>
	typename std::enable_if<std::is_same<Q, bar>::value, bool>::type check()
	{
		return true;
	}

	template<class Q = T>
	typename std::enable_if<!std::is_same<Q, bar>::value, bool>::type check()
	{
		return false;
	}
};

int main()
{
	is_bar<foo> foo_is_bar;
	is_bar<bar> bar_is_bar;
	if (!foo_is_bar.check() && bar_is_bar.check())
		std::cout << "It works!" << std::endl;

	return 0;
}
*/
int main() {

	AddOne adder;
	Queue<int> inputQueue, outputQueue;
	Source source(inputQueue);
	Sink sink(outputQueue);
	ExecutionPool<int, int> pool(inputQueue, outputQueue, 3);

	std::thread thSource(std::ref(source));
	std::thread thSink(std::ref(sink));
	std::thread thPool(std::ref(pool), adder);

	thSource.join();
	thSink.join();
	thPool.join();
	return 0;
}
