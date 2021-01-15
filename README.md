# Concurrency
[![Build Status](https://travis-ci.org/RealTimeBiomechanics/Concurrency.svg?branch=develop)](https://travis-ci.org/RealTimeBiomechanics/Concurrency)

Concurrency is a lightweight portable library to handle thread synchronization through an efficient C++14 implementation of Queues and Latches. 
Concurrency enables using a few lines of code to implement the Producer-Consumer and Publisher-Subsribers problems. 

## Example Publisher Subscriber
```cpp
#include <thread>
#include <iostream>
#include "rtb/concurrency/Concurrency.h"
using rtb::Concurrency::Queue;

Queue<int> q;

void produce(int n) {
    for (int i{ 0 }; i < n; ++i) {
        q.push(i);
    }
    q.close();
}

void consume() {
	q.subscribe();
	while (true) {
		auto val = q.pop();
		if (!val.has_value()) break;
		cout << val.value() << endl;
    }
    q.unsubscribe();
}

int main() {
    std::thread prodThr(&produce, 10);
    std::thread consThr(&consume);

    prodThr.join();
    consThr.join();

    return 0;
}
```

Check here for some further [examples](example).


## Requirements

* Cross-platform building: [CMake](http://www.cmake.org/) 3.1.0 or later
* Compiler:
   * [Visual Studio](http://www.visualstudio.com) 2019 or later (Windows only)
   * [gcc](http://gcc.gnu.org/) 9.0 or later (typically on Linux)

Previous versions of compilers might work but have not been fully tested. It should work in Clang on Mac, but it has not been tested.

## How to compile

Concurrency is a cross-platform library that relies on the  [CMake](http://cmake.org/) build system.
Please refer to [official CMake documentation](https://cmake.org/runningcmake/) for instructions on how to
configure and build this library on the operating system and with the compiler/build environment that you are using.

Briefly, you can follow these instruction for compile Concurrency in Linux and Windows.

### Linux

From the Concurrency directory run the following code

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j2
make install
```

### Windows

Using Git Bash and from the Concurrency directory run the following code

```bash
mkdir build
cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release --target install
```
After installation, create a Windows environment variable names `Concurrency_DIR` which points at the installation directory. This will be used to eaily import Concurrency in your own project.

## Use Concurrency in your own project

Once installed, you can use CMake to automatically find the Concurrency package

```cmake
find_package(Concurrency REQUIRED)
```

[Quickstart](example/quickstart) the development of your project using Concurrency.

## License

Please see the file called LICENSE.txt.

Copyright (c) 2020 M. Reggiani, C. Pizzolato

Licensed under the Apache License, Version 2.0 (the "License").
You may not use this file except in compliance with the License.
You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License
is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.


## Contacts

For help on building and using the Concurrency library, for bug reports,
and for questions or suggestions use the GitHub issue tracking system.

## Cite us

If you are using Concurrency for your academic work, pleaser cosider citing the following paper

Pizzolato, C., Reggiani, M., Modenese, L., & Lloyd, D. G. (2017). Real-time inverse kinematics and inverse dynamics for lower limb applications using OpenSim. Comput Methods Biomech Biomed Engin, 20(4), 436-445. [doi:10.1080/10255842.2016.1240789](https://pubmed.ncbi.nlm.nih.gov/27723992/)

