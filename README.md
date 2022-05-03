# Concurrency

Concurrency is a small library to handle thread synchronization through an efficient C++11 implementation of Queues and Latches.

Some basic examples on how to use Concurrency library are provided in the
[tests](https://github.com/RealTimeBiomechanics/Concurrency/tree/master/tests) folder.

The latest version of the code, together with manuals and terms of use can be found at: https://github.com/RealTimeBiomechanics/Concurrency



## Dependencies

Concurrency depends on the following

* Cross-platform building: [CMake](http://www.cmake.org/) 3.1.0 or later
* Compiler:
   * [Visual Studio](http://www.visualstudio.com) 2012 or later (Windows only)
   * [gcc](http://gcc.gnu.org/) 4.8.1 or later (typically on Linux)
   * [Clang](http://clang.llvm.org/) 3.4 or later (typically on Mac, possibly through Xcode)


## How to compile

Concurrency is a cross-platform library that relies on the  [CMake](http://cmake.org/) build system.
Please refer to [official CMake documentation](https://cmake.org/runningcmake/) for instructions on how to
configure and build this library on the operating system and with the compiler/build environment that you are using.


## Example Producer Consumer

```c++
#include <thread>
#include <iostream>
#include "rtb/concurrency/Concurrency.h"

using rtb::Concurrency::Queue;
using rtb::Concurrency::Latch;

Queue<int> q;
Latch latch(2); // Create a latch that will synchronize two threads

void produce(int n) {

    latch.wait(); // Block until shared latch lets it through

    for (int i = n-1; i >= 0; i--) {
        q.push(i);
    }
}

void consume() {

    q.subscribe();

    latch.wait();

    int val;
    do {
        val = q.pop();
        std::cout << val << std::endl;
    } while (val > 0);

    q.unsubscribe();
}

int main() {

    std::thread thread_produce(&produce, 10);
    std::thread thread_consume(&consume);

    thread_produce.join();
    thread_consume.join();

    return 0;
}
```

## License

Please see the file called LICENSE.txt.

Copyright (c) 2014 M. Reggiani

Licensed under the Apache License, Version 2.0 (the "License").
You may not use this file except in compliance with the License.
You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License
is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.


## Contacts

For help on building and using the Concurrency library, for bug reports,
and for questions or suggestions please send an e-mail to monica.reggiani@gmail.com.
