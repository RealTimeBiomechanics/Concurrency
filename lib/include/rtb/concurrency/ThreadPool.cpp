#include <vector>
#include <thread>

namespace rtb {
	namespace Concurrency {

		template<typename InputData, typename OutputData>
		ExecutionPool<InputData, OutputData>::ExecutionPool(InputQueue& inputQueue, OutputQueue& outputQueue, unsigned numberOfWorkers) :
			inputQueue_(inputQueue),
			outputQueue_(outputQueue),
			numberOfWorkers_(numberOfWorkers)

		{}

		template<typename InputData, typename OutputData>
		template<typename Funct, typename... Args>
		void ExecutionPool<InputData, OutputData>::operator()(Funct funct, Args... args) {

			IndexedDataQueue<InputData> jobsQueue;
			SortedIndexedDataQueue<InputData> processedJobsQueue;
			SimpleQueue<IndexT> sequenceQueue;

			JobsCreator<InputData> jobCreator(inputQueue_, jobsQueue, sequenceQueue);
			MessageSorter<OutputData> messageSorter(processedJobsQueue, sequenceQueue, outputQueue_);
			std::vector<std::shared_ptr<Worker<Funct>>> workers;
			for (unsigned i(0); i < numberOfWorkers_; ++i) {
				workers.emplace_back(std::make_shared<Worker<Funct>>(jobsQueue, processedJobsQueue, funct));
			}
	
			std::vector<std::thread> workersThreads;
			for (auto& it : workers)
				workersThreads.emplace_back(std::ref(*it), std::forward<Args>(args)...);

			std::thread jobCreatorThread(std::ref(jobCreator));
			std::thread messageSorterThread(std::ref(messageSorter));

			for (auto& it : workersThreads)
				it.join();

			jobCreatorThread.join();
			messageSorterThread.join();
		}

		//to factorize this with the other operator()() without latch
		template<typename InputData, typename OutputData>
		template<typename Funct, typename... Args>
		void ExecutionPool<InputData, OutputData>::operator()(Latch& latch, Funct funct, Args... args) {

			IndexedDataQueue<InputData> jobsQueue;
			SortedIndexedDataQueue<InputData> processedJobsQueue;
			SimpleQueue<IndexT> sequenceQueue;

			JobsCreator<InputData> jobCreator(inputQueue_, jobsQueue, sequenceQueue);
			MessageSorter<OutputData> messageSorter(processedJobsQueue, sequenceQueue, outputQueue_);
			std::vector<std::shared_ptr<Worker<Funct>>> workers;
			for (unsigned i(0); i < numberOfWorkers_; ++i) {
				workers.emplace_back(std::make_shared<Worker<Funct>>(jobsQueue, processedJobsQueue, funct));
			}

			latch.wait();
			std::vector<std::thread> workersThreads;
			for (auto& it : workers)
				workersThreads.emplace_back(std::ref(*it), std::forward<Args...>(args));

			std::thread jobCreatorThread(std::ref(jobCreator));
			std::thread messageSorterThread(std::ref(messageSorter));

			for (auto& it : workersThreads)
				it.join();

			jobCreatorThread.join();
			messageSorterThread.join();

		}

		template<typename Funct>
		Worker<Funct>::Worker(InputQueue& inputQueue, OutputQueue& outputQueue, Funct funct) :
			inputQueue_(inputQueue),
			outputQueue_(outputQueue),
			funct_(funct)
		{}


		template<typename Funct>
		template<typename... Args>
		void Worker<Funct>::operator()(Args... args) {

			while (true) {
				IndexedData<InputData> inData = inputQueue_.pop();
				auto functOutput =  funct_(std::get<1>(inData), std::forward<Args>(args)...);
				IndexedData<OutputData> outData;
				std::get<0>(outData) = std::move(std::get<0>(inData));
				std::get<1>(outData) = std::move(functOutput);
				outputQueue_.push(outData);
			}
		}


		template<typename T>
		JobsCreator<T>::JobsCreator(
			Queue<T>& inputQueue,
			IndexedDataQueue<T>& outputJobsQueue,
			IndexQueue& outputSequenceQueue) :
			inputQueue_(inputQueue),
			outputJobsQueue_(outputJobsQueue),
			outputSequenceQueue_(outputSequenceQueue),
			idx_(0)
		{}

		template<typename T>
		void JobsCreator<T>::operator()() {

			inputQueue_.subscribe();
			while (true) {
				auto data{ inputQueue_.pop() };
				IndexedData<T> iData{ idx_, std::move(data) };
				outputJobsQueue_.push(iData);
				outputSequenceQueue_.push(idx_);
				++idx_;
			}
		}

		template<typename T>
		MessageSorter<T>::MessageSorter(
			SortedIndexedDataQueue<T>& inputFromThreadPool,
			IndexQueue& inputSequence,
			Queue<T>& outputQueue) :
			inputFromThreadPool_(inputFromThreadPool),
			inputSequence_(inputSequence),
			outputQueue_(outputQueue)
		{}

		template<typename T>
		void MessageSorter<T>::operator()() {

			while (true) {
				IndexT idx{ inputSequence_.pop() };
				auto data{ inputFromThreadPool_.pop_index(idx) };
				outputQueue_.push(std::get<1>(data));
			}

		}


	}
}
