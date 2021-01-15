#include <vector>
#include <thread>
#include <utility>

namespace rtb {
namespace Concurrency {

    template<typename InputData, typename OutputData>
    ExecutionPool<InputData, OutputData>::ExecutionPool(InputQueue &inputQueue,
        OutputQueue &outputQueue,
        unsigned numberOfWorkers)
        : inputQueue_(inputQueue)
        , outputQueue_(outputQueue)
        , numberOfWorkers_(numberOfWorkers)

    {}

    template<typename InputData, typename OutputData>
    template<typename Funct, typename... Args>
    void ExecutionPool<InputData, OutputData>::operator()(Funct funct, Args... args) {
        IndexedDataQueue<InputData> jobsQueue;
        SortedIndexedDataQueue<InputData> processedJobsQueue;
        SimpleQueue<IndexT> sequenceQueue;
        Latch internalLatch(numberOfWorkers_ + 3);
        JobsCreator<InputData> jobCreator(inputQueue_, jobsQueue, sequenceQueue, internalLatch, numberOfWorkers_);
        MessageSorter<OutputData> messageSorter(
            processedJobsQueue, sequenceQueue, outputQueue_, internalLatch);
        std::vector<std::shared_ptr<Worker<Funct>>> workers;
        for (unsigned i(0); i < numberOfWorkers_; ++i) {
            workers.emplace_back(
                std::make_shared<Worker<Funct>>(jobsQueue, processedJobsQueue, internalLatch, funct));
        }

        std::vector<std::thread> workersThreads;
        for (auto &it : workers)
            workersThreads.emplace_back(std::ref(*it), std::forward<Args>(args)...);

        std::thread jobCreatorThread(std::ref(jobCreator));
        std::thread messageSorterThread(std::ref(messageSorter));
        internalLatch.wait();
        for (auto &it : workersThreads)
            it.join();

        jobCreatorThread.join();
        messageSorterThread.join();
    }

    // to factorize this with the other operator()() without latch
    template<typename InputData, typename OutputData>
    template<typename Funct, typename... Args>
    void ExecutionPool<InputData, OutputData>::operator()(Latch &latch, Funct funct, Args... args) {
        IndexedDataQueue<InputData> jobsQueue;
        SortedIndexedDataQueue<InputData> processedJobsQueue;
        SimpleQueue<IndexT> sequenceQueue;

        Latch internalLatch(numberOfWorkers_+3);
        JobsCreator<InputData> jobCreator(inputQueue_, jobsQueue, sequenceQueue, internalLatch, numberOfWorkers_);
        MessageSorter<OutputData> messageSorter(
            processedJobsQueue, sequenceQueue, outputQueue_, internalLatch);
        std::vector<std::shared_ptr<Worker<Funct>>> workers;
        for (unsigned i(0); i < numberOfWorkers_; ++i) {
            workers.emplace_back(std::make_shared<Worker<Funct>>(
                jobsQueue, processedJobsQueue, internalLatch, funct));
        }

        latch.wait();
        std::vector<std::thread> workersThreads;
        for (auto &it : workers)
            workersThreads.emplace_back(std::ref(*it), std::forward<Args>(args)...);

        std::thread jobCreatorThread(std::ref(jobCreator));
        std::thread messageSorterThread(std::ref(messageSorter));
        internalLatch.wait();
        for (auto &it : workersThreads)
            it.join();

        jobCreatorThread.join();
        messageSorterThread.join();
    }

    template<typename Funct>
    Worker<Funct>::Worker(InputQueue &inputQueue,
        OutputQueue &outputQueue,
        Latch &latch,
        Funct funct)
        : inputQueue_(inputQueue)
        , outputQueue_(outputQueue)
        , latch_(latch)
        , funct_(funct) {}

    template<typename Funct>
    template<typename... Args>
    void Worker<Funct>::operator()(Args... args) {
        latch_.wait();
        decltype(inputQueue_.pop()) inData;
        while(true) {
            inData = inputQueue_.pop();
            if (!inData.has_value()) break;
            auto functOutput = funct_(std::get<1>(inData.value()), std::forward<Args>(args)...);
            IndexedData<OutputData> outData;
            std::get<0>(outData) = std::get<0>(inData.value());
            std::get<1>(outData) = std::move(functOutput);
            outputQueue_.push(outData);
        }
        outputQueue_.close();
    }

    template<typename T>
    JobsCreator<T>::JobsCreator(Queue<T> &inputQueue,
        IndexedDataQueue<T> &outputJobsQueue,
        IndexQueue &outputSequenceQueue,
        Latch &latch,
        unsigned numberOfWorkers)
        : inputQueue_(inputQueue)
        , outputJobsQueue_(outputJobsQueue)
        , outputSequenceQueue_(outputSequenceQueue)
        , latch_(latch)
        , idx_(0)
        , numberOfWorkers_(numberOfWorkers) {}

    template<typename T>
    void JobsCreator<T>::operator()() {
        inputQueue_.subscribe();
        latch_.wait();
        while (true) {
            auto data{ inputQueue_.pop() };
            if (!data.has_value()) break;
            IndexedData<T> iData{ idx_, std::move(data.value()) };
            outputJobsQueue_.push(iData);
            outputSequenceQueue_.push(idx_);
            ++idx_;
        }
        for (unsigned i(0); i < numberOfWorkers_; ++i)
            outputJobsQueue_.close();
        outputSequenceQueue_.close();
        inputQueue_.unsubscribe();
    }

    template<typename T>
    MessageSorter<T>::MessageSorter(SortedIndexedDataQueue<T> &inputFromThreadPool,
        IndexQueue &inputSequence,
        Queue<T> &outputQueue,
        Latch &latch)
        : inputFromThreadPool_(inputFromThreadPool)
        , inputSequence_(inputSequence)
        , outputQueue_(outputQueue)
        , latch_(latch) {}

    template<typename T>
    void MessageSorter<T>::operator()() {
        latch_.wait();
        while (true) {
            auto inputSequenceResult{ inputSequence_.pop() };
            if (!inputSequenceResult.has_value()) break;
            IndexT idx{ inputSequenceResult.value() };
            auto val{ inputFromThreadPool_.popIndex(idx) };
            if (val.has_value()) { outputQueue_.push(std::get<1>(val.value())); }
        }
        outputQueue_.close();
    }

}// namespace Concurrency
}// namespace rtb
