/* -------------------------------------------------------------------------- *
 * Copyright (c) 2020      C. Pizzolato, M. Reggiani                          *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at:                                   *
 * http://www.apache.org/licenses/LICENSE-2.0                                 *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */
#ifndef rtb_ThreadPool_h
#define rtb_ThreadPool_h

#include "rtb/concurrency/Queue.h"
#include "rtb/concurrency/SimpleQueue.h"
#include "rtb/concurrency/Latch.h"
#include <queue>
#include <tuple>
#include <memory>
#include <variant>

namespace rtb {

namespace Concurrency {

    template<typename T>
    class JobsCreator {
        /* Tags each of the input messages with a unique identifier
         * dispatch messages to each of the workers and stores the order of the messages to
         * `outputSequenceQueue`
         *
         */
      public:
        JobsCreator() = delete;
        JobsCreator(JobsCreator &) = delete;
        JobsCreator(Queue<T> &inputQueue,
            IndexedDataQueue<T> &outputJobsQueue,
            IndexQueue &outputSequenceQueue,
            Latch & latch,
            unsigned numberOfWorkers);
        void operator()();

      private:
        Queue<T> &inputQueue_;
        IndexedDataQueue<T> &outputJobsQueue_;
        IndexQueue &outputSequenceQueue_;
        IndexT idx_;
        Latch &latch_;
        unsigned numberOfWorkers_;
    };

    template<typename T>
    class MessageSorter {
        /* Message sorter reorganise the messages produced by the ThreadPool
         * in the correct temporal sequence.
         */
      public:
        MessageSorter() = delete;
        MessageSorter(SortedIndexedDataQueue<T> &inputFromThreadPool,
            IndexQueue &inputSequence,
            Queue<T> &outputQueue,
            Latch& latch);
        void operator()();

      private:
        SortedIndexedDataQueue<T> &inputFromThreadPool_;
        IndexQueue &inputSequence_;
        Queue<T> &outputQueue_;
        Latch &latch_;
    };

    template<typename Funct>
    class Worker {
      public:
        using InputData = typename Funct::InputData;
        using OutputData = typename Funct::OutputData;
        using InputQueue = IndexedDataQueue<InputData>;
        using OutputQueue = SortedIndexedDataQueue<OutputData>;
        Worker(InputQueue &inputQueue, OutputQueue &outputQueue, Latch& latch, Funct funct);
        template<typename... Args>
        void operator()(Args... args);

      private:
        InputQueue &inputQueue_;
        OutputQueue &outputQueue_;
        Latch &latch_;
        Funct funct_;
    };

    template<typename InputData, typename OutputData>
    class ExecutionPool {
      public:
        using InputQueue = Queue<InputData>;
        using OutputQueue = Queue<OutputData>;
        ExecutionPool() = delete;
        ExecutionPool(ExecutionPool &) = delete;
        ExecutionPool(InputQueue &inputQueue, OutputQueue &outputQueue, unsigned numberOfWorkers);

        template<typename Funct, typename... Args>
        void operator()(Funct funct, Args... args);
        template<typename Funct, typename... Args>
        void operator()(Latch &latch, Funct funct, Args... args);

      private:
        InputQueue &inputQueue_;
        OutputQueue &outputQueue_;
        unsigned numberOfWorkers_;
    };

    template<typename InputData, typename OutputData>
    auto makeExecutionPool(
        Queue<InputData> &inputQueue,
        Queue<OutputData> &outputQueue,
        unsigned numberOfWorkers) {

        return std::make_shared < ExecutionPool<InputData, OutputData>>
            (inputQueue, outputQueue, numberOfWorkers);
    }

}// namespace Concurrency
}// namespace rtb
#include "ThreadPool.cpp"
#endif