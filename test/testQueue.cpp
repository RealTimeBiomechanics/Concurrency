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
#include "threadFunctions.h"
#include <iostream>
#include <vector>
#include <functional>

using namespace rtb::Concurrency;
using std::ref;

int test1() {
    // FIRST TEST
    // Producer start first (at 10) and send 100 msg with a period of 10
    // 4 Consumers subscribe before (at 5) and unsubscribe (at 1500)
    // OUTPUT: all the consumers should read everything

    std::cout << "\n ---------------- First Test ---------------- \n";
    std::cout << "OUTPUT:  all the consumers read 100 messages\n\n";

    std::vector<TimeT> subscribeTime = { TimeT{ 5 } };
    std::vector<TimeT> unsubscribeTime = { TimeT{ 1500 } };

    Queue<int> q;

    Producer prod;
    Consumer cons1, cons2, cons3, cons4;

    // producer thread
    std::thread prodThr(ref(prod), ref(q), TimeT{ 10 }, TimeT{ 10 }, 100);

    // consumer threads
    std::thread consumer1Thr(ref(cons1), ref(q), 1, subscribeTime, unsubscribeTime, TimeT{ 0 });
    std::thread consumer2Thr(ref(cons2), ref(q), 2, subscribeTime, unsubscribeTime, TimeT{ 0 });
    std::thread consumer3Thr(ref(cons3), ref(q), 3, subscribeTime, unsubscribeTime, TimeT{ 0 });
    std::thread consumer4Thr(ref(cons4), ref(q), 4, subscribeTime, unsubscribeTime, TimeT{ 0 });

    prodThr.join();
    consumer1Thr.join();
    consumer2Thr.join();
    consumer3Thr.join();
    consumer4Thr.join();

    bool areEqual = (prod.producedValues == cons1.consumedValues)
                    && (prod.producedValues == cons2.consumedValues)
                    && (prod.producedValues == cons3.consumedValues)
                    && (prod.producedValues == cons4.consumedValues);

    return areEqual;
}

int test2() {
    // SECOND TEST
    // Producer start first (at 10) and send 100 msg with a period of 10
    // Consumer 1 and 2 subscribe before (at 5),
    // Consumer 3 and 4 subscribe after (at 500) and read half of the messages
    // Consumer 5 and 6 subscribe before (at 5) andn leave in the middle
    // OUTPUT: Consumers 1 and 2 100 messages
    //         Consumers 3 and 4 roughly the last half of the messages
    //         Consumers 5 and 6 roughly the first half of the messages

    std::cout << "\n ---------------- Second Test ----------------\n";
    std::cout << "OUTPUT: Consumers 1 and 2 read messages from 1 to 100\n";
    std::cout << "        Consumers 3 and 4 approximatively from 50 to 100\n";
    std::cout << "        Consumers 5 and 6 approximatively from 1 to 50\n\n";

    Queue<int> q;

    std::vector<TimeT> subscribeTimeConsumer1and2 = { TimeT{ 5 } };
    std::vector<TimeT> unsubscribeTimeConsumer1and2 = { TimeT{ 1500 } };
    std::vector<TimeT> subscribeTimeConsumer3and4 = { TimeT{ 500 } };
    std::vector<TimeT> unsubscribeTimeConsumer3and4 = { TimeT{ 1500 } };
    std::vector<TimeT> subscribeTimeConsumer5and6 = { TimeT{ 5 } };
    std::vector<TimeT> unsubscribeTimeConsumer5and6 = { TimeT{ 510 } };

    Producer prod;
    Consumer cons1, cons2, cons3, cons4, cons5, cons6;

    // producer thread
    std::thread prodThr(ref(prod), ref(q), TimeT{ 10 }, TimeT{ 10 }, 100);

    // consumer threads
    std::thread consumer1Thr(ref(cons1),
        ref(q),
        1,
        subscribeTimeConsumer1and2,
        unsubscribeTimeConsumer1and2,
        TimeT{ 0 });

    std::thread consumer2Thr(ref(cons2),
        ref(q),
        2,
        subscribeTimeConsumer1and2,
        unsubscribeTimeConsumer1and2,
        TimeT{ 0 });
    std::thread consumer3Thr(ref(cons3),
        ref(q),
        3,
        subscribeTimeConsumer3and4,
        unsubscribeTimeConsumer3and4,
        TimeT{ 0 });
    std::thread consumer4Thr(ref(cons4),
        ref(q),
        4,
        subscribeTimeConsumer3and4,
        unsubscribeTimeConsumer3and4,
        TimeT{ 0 });
    std::thread consumer5Thr(ref(cons5),
        ref(q),
        5,
        subscribeTimeConsumer5and6,
        unsubscribeTimeConsumer5and6,
        TimeT{ 0 });
    std::thread consumer6Thr(ref(cons6),
        ref(q),
        6,
        subscribeTimeConsumer5and6,
        unsubscribeTimeConsumer5and6,
        TimeT{ 0 });

    prodThr.join();
    consumer1Thr.join();
    consumer2Thr.join();
    consumer3Thr.join();
    consumer4Thr.join();
    consumer5Thr.join();
    consumer6Thr.join();

    bool success = true;
    //
    success = (prod.producedValues == cons1.consumedValues)
              && (prod.producedValues == cons2.consumedValues);

    size_t c3Size = cons3.consumedValues.size();
    success &= (c3Size > 45);
    size_t pSize = prod.producedValues.size();
    std::vector<int> secondPart(
        prod.producedValues.begin() + (pSize - c3Size), prod.producedValues.end());
    success &= (secondPart == cons3.consumedValues);

    size_t c5Size = cons5.consumedValues.size();
    success &= (c5Size < 55);
    std::vector<int> firstPart(prod.producedValues.begin(), prod.producedValues.begin() + c5Size);
    success &= (firstPart == cons5.consumedValues);

    return success;
}

int test3() {
    // THIRD TEST
    // Producer start first (at 10) and send 100 msg with a period of 10
    // OUTPUT: Consumers 1 and 2  1 to 25 and 50 to 75
    //         Consumers 3 and 4  25 to 50 and  75 to 100

    std::cout << "\n ---------------- Third Test ----------------\n";
    std::cout << "OUTPUT: Consumers 1 and 2 aapproximatively  from 1 to 25 and 50 to 75\n";
    std::cout << "        Consumers 3 and 4 approximatively from 25 to 50 and 75 to 100\n\n";

    Queue<int> q;

    std::vector<TimeT> subscribeTimeConsumer1and2 = { TimeT{ 5 }, TimeT{ 500 } };
    std::vector<TimeT> unsubscribeTimeConsumer1and2 = { TimeT{ 250 }, TimeT{ 760 } };

    std::vector<TimeT> subscribeTimeConsumer3and4 = { TimeT{ 250 }, TimeT{ 750 } };
    std::vector<TimeT> unsubscribeTimeConsumer3and4 = { TimeT{ 500 }, TimeT{ 1500 } };

    Producer prod;
    Consumer cons1, cons2, cons3, cons4;
    // producer thread
    std::thread prodThr(ref(prod), ref(q), TimeT{ 10 }, TimeT{ 10 }, 100);

    // consumer threads
    std::thread consumer1Thr(ref(cons1),
        ref(q),
        1,
        subscribeTimeConsumer1and2,
        unsubscribeTimeConsumer1and2,
        TimeT{ 0 });
    std::thread consumer2Thr(ref(cons2),
        ref(q),
        2,
        subscribeTimeConsumer1and2,
        unsubscribeTimeConsumer1and2,
        TimeT{ 0 });
    std::thread consumer3Thr(ref(cons3),
        ref(q),
        3,
        subscribeTimeConsumer3and4,
        unsubscribeTimeConsumer3and4,
        TimeT{ 0 });
    std::thread consumer4Thr(ref(cons4),
        ref(q),
        4,
        subscribeTimeConsumer3and4,
        unsubscribeTimeConsumer3and4,
        TimeT{ 0 });

    prodThr.join();
    consumer1Thr.join();
    consumer2Thr.join();
    consumer3Thr.join();
    consumer4Thr.join();
    // Assess whether the re-subscription was sucessful
    bool success = cons1.consumedValues.size() > 40;
    success &= cons2.consumedValues.size() > 40;
    success &= cons3.consumedValues.size() > 40;
    success &= cons4.consumedValues.size() > 40;
    return success;
}

int test4() {
    // FOURTH TEST
    // This test check if unread messages from the leaving client (the slowest one) are removed from
    // the queue. Producer start first (at 10) and send 100 msg with a period of 10. Three
    // consumers: First one goes on for the whole time reading all the messages Second one (slowest)
    // stop at 500 and removes all the messages still to be read The third one arrives later (at
    // 700) and should start reading aroung message 70

    std::cout << "\n ---------------- Fourth Test ---------------- \n";
    std::cout << "OUTPUT: Consumers 1 read all the 100 messages\n";
    std::cout << "        Consumers 2 (the slowest one) stop at 500 but should read half of the "
                 "available\n";
    std::cout << "        Consumers 3 approximatively  from message 70\n\n";

    Queue<int> q;

    std::vector<TimeT> subscribeTimeConsumer1 = { TimeT{ 5 } };
    std::vector<TimeT> unsubscribeTimeConsumer1 = { TimeT{ 1500 } };

    std::vector<TimeT> subscribeTimeConsumer2 = { TimeT{ 5 } };
    std::vector<TimeT> unsubscribeTimeConsumer2 = { TimeT{ 500 } };

    std::vector<TimeT> subscribeTimeConsumer3 = { TimeT{ 700 } };
    std::vector<TimeT> unsubscribeTimeConsumer3 = { TimeT{ 1500 } };

    Producer prod;
    Consumer cons1, cons2, cons3, cons4;

    // producer thread
    std::thread prodThr(ref(prod), ref(q), TimeT{ 10 }, TimeT{ 10 }, 100);

    // consumer threads
    std::thread consumer1Thr(
        ref(cons1), ref(q), 1, subscribeTimeConsumer1, unsubscribeTimeConsumer1, TimeT{ 0 });
    std::thread consumer2Thr(
        ref(cons2), ref(q), 2, subscribeTimeConsumer2, unsubscribeTimeConsumer2, TimeT{ 20 });
    std::thread consumer3Thr(
        ref(cons3), ref(q), 3, subscribeTimeConsumer3, unsubscribeTimeConsumer3, TimeT{ 0 });

    prodThr.join();
    consumer1Thr.join();
    consumer2Thr.join();
    consumer3Thr.join();

    bool success = cons1.consumedValues == prod.producedValues;
    success &= cons3.consumedValues.front() > cons2.consumedValues.back() + 30;
    return success;
}

int test5() {
    // FIFTH TEST
    // Producer start first (at 10) and send 100 msg with a period of 10
    // Consumers start after the last msg has been sent
    // OUTPUT: Consumers block

    std::cout << "\n ---------------- Fifth Test ---------------- \n";
    std::cout << " Consumers start after the producer has left\n";
    std::cout << " OUTPUT: Consumers block trying to pop the queue.\n\n";

    Queue<int> q;

    std::vector<TimeT> subscribeTime = { TimeT{ 1500 } };
    std::vector<TimeT> unsubscribeTime = { TimeT{ 3000 } };

    Producer prod;
    Consumer cons1, cons2, cons3, cons4;

    // producer thread
    std::thread prodThr(ref(prod), std::ref(q), TimeT{ 10 }, TimeT{ 10 }, 100);

    // consumer threads
    std::thread consumer1Thr(ref(cons1), ref(q), 1, subscribeTime, unsubscribeTime, TimeT{ 0 });
    std::thread consumer2Thr(ref(cons2), ref(q), 2, subscribeTime, unsubscribeTime, TimeT{ 0 });
    std::thread consumer3Thr(ref(cons3), ref(q), 3, subscribeTime, unsubscribeTime, TimeT{ 0 });
    std::thread consumer4Thr(ref(cons4), ref(q), 4, subscribeTime, unsubscribeTime, TimeT{ 0 });

    auto killswitch([&]() {
        // wait 10 seconds
        std::this_thread::sleep_for(TimeT{ 10000 });
        // Send a value to check if the killswitch function was responsible for closing the
        // consumers
        q.push(-1);
        // Send the teminate command to all the consumers
        q.push(std::numeric_limits<int>::max());
    });
    killswitch();
    prodThr.join();
    consumer1Thr.join();
    consumer2Thr.join();
    consumer3Thr.join();
    consumer4Thr.join();
    bool success = cons1.consumedValues.front() == -1;
    success &= cons2.consumedValues.front() == -1;
    success &= cons3.consumedValues.front() == -1;
    success &= cons4.consumedValues.front() == -1;
    
    return success;
}

int main() {
    if (!test1()) {
        std::cout << "Test1 failed\n";
        return 1;
    }
    if (!test2()) {
        std::cout << "Test2 failed\n";
        return 1;
    }
    if (!test3()) {
        std::cout << "Test3 failed\n";
        return 1;
    }
    if (!test4()) {
        std::cout << "Test4 failed\n";
        return 1;
    }
    if (!test5()) {
        std::cout << "Test5 failed\n";
        return 1;
    }

    return 0;
}
