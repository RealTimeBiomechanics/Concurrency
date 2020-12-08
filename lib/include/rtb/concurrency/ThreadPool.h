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
#include <concepts>
#include <memory>

namespace rtb {

    namespace Concurrency {

        using IndexQueue = SimpleQueue<IndexT>;
        
        template<typename T>
        using IndexedDataQueue = SimpleQueue<IndexedData<T>>;

        template<typename T>
        using SortedIndexedDataQueue = SimpleQueue<IndexedData<T>, std::priority_queue<IndexedData<T>>>;

        template<typename T>
        class JobsCreator {
            /* Tags each of the input messages with a unique identifier
            * dispatch messages to each of the workers and stores the order of the messages to `outputSequenceQueue`
            *
            */
        public:
            JobsCreator() = delete;
            JobsCreator(JobsCreator&) = delete;
            JobsCreator(
                Queue<T>& inputQueue,
                IndexedDataQueue<T>& outputJobsQueue,
                IndexQueue& outputSequenceQueue);
            void operator()();
        private:
            Queue<T>& inputQueue_;
            IndexedDataQueue<T>& outputJobsQueue_;
            IndexQueue& outputSequenceQueue_;
            IndexT idx_;
        };

        template<typename T>
        class MessageSorter {
            /* Message sorter reorganize the messages produced by the ThreadPool
            * in the correct temporal sequence.
            */
        public:
            MessageSorter() = delete;
            MessageSorter(
                SortedIndexedDataQueue<T>& inputFromThreadPool,
                IndexQueue& inputSequence,
                Queue<T>& outputQueue);
            void operator()();
        private:
            SortedIndexedDataQueue<T>& inputFromThreadPool_;
            IndexQueue& inputSequence_;
            Queue<T>& outputQueue_;
        };


        //  template<typename T>
        //  concept Worker = requires {
        //      typename T::InputQueue;
        //      typename T::OutputQueue;
        //  };

      //    template<Worker T>
       /*   template<typename Worker>
          class ExecutionPool {
          public:
              using InputData = typename Worker::InputQueue::Data;
              using OutputData = typename Worker::OutputQueue::Data;
              template<typename... Args>
              ThreadPool(SimpleQueue<IndexedData<InputData>>&& inputQueue, SimpleQueue<IndexedData<OutputData>>& outputQueue, Args ... args);

          private:
              SimpleQueue<IndexedData<InputData>>& inputQueue_;
              SimpleQueue<OutputData, PriorityQueue<OutputData>>& outputQueue_;
          };
      */


        template<typename Funct>
        class Worker {
          
        public:
            using InputData = typename Funct::InputData;
            using OutputData = typename Funct::OutputData;
            using InputQueue = IndexedDataQueue<InputData>;
            using OutputQueue = SortedIndexedDataQueue<OutputData>;
            Worker(InputQueue& inputQueue, OutputQueue& outputQueue, Funct funct);
            template<typename... Args>
            void operator()(Args... args);
           
        private:
            InputQueue& inputQueue_;
            OutputQueue& outputQueue_;
            Funct funct_;
        };


        template<typename InputData, typename OutputData>
        class ExecutionPool {
        public:
            using InputQueue = Queue<InputData>;
            using OutputQueue = Queue<OutputData>;
            ExecutionPool() = delete;
            ExecutionPool(ExecutionPool&) = delete;
 
            ExecutionPool(InputQueue& inputQueue, OutputQueue& outputQueue, unsigned numberOfWorkers);
          
            template<typename Funct, typename... Args>
            void operator()(Funct funct, Args... args);
            template<typename Funct, typename... Args>
            void operator()(Latch& latch, Funct funct, Args... args);

        private:
            InputQueue& inputQueue_;
            OutputQueue& outputQueue_;
            unsigned numberOfWorkers_;
        };
    }
}
#include "ThreadPool.cpp"
#endif