/* -------------------------------------------------------------------------- *
 * Copyright (c) 2014      M. Reggiani                                        *
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
#include "rtb/concurrency/Latch.h"
#include <iostream>
#include <algorithm>

#include <iostream>
#include <algorithm>


namespace rtb{
    namespace Concurrency{

        Latch::Latch()
            :count_(0)
        { }


        Latch::Latch(int count)
            : count_(count)
        { }


        void Latch::setCount(int count)
        {
            if (count_ != 0) {
                std::cout << "You are not allowed to reset a Latch\n";
                exit(EXIT_FAILURE);
            }
            else
                count_ = count;
        }


		void Latch::increaseCount(unsigned n)
		{
			std::unique_lock<std::mutex> mlock(mutex_);
			count_ += n;
			mlock.unlock();
		}

        void Latch::wait() {
            std::unique_lock<std::mutex> mlock(mutex_);
            if (count_ == 0) {
                throw std::logic_error("internal count == 0");
            }
            if (--count_ == 0)
                condition_.notify_all();
            else {
                while (count_ > 0)
                    condition_.wait(mlock);
            }
            mlock.unlock();
        }
    }
}
