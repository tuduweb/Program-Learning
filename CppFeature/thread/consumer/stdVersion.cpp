#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
//#include <vector>
#include <queue>
#include <condition_variable>

std::mutex g_mutex;

std::queue<int> g_msgQueue;
std::condition_variable g_condVar;

void producer(int start, int end) {
    for(int x = start; x < end; ++x) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        {
            std::lock_guard<std::mutex> guard(g_mutex);
            g_msgQueue.push(x);
        }
        //std::cout << "Producer message" << x << std::endl;
        printf("Producer create %d\r\n", x);
        g_condVar.notify_all();
    }
}

void consumer(int demand) {
    while(true) {
        std::unique_lock<std::mutex> ulock(g_mutex);
        g_condVar.wait(ulock, []{ return g_msgQueue.size() > 0; }); // wait的第二个参数使得显式的double check不再必要
        //std::cout << "Consumer message " << g_msgQueue.front() << std::endl;
        printf("Consumer message %d\r\n", g_msgQueue.front());
        g_msgQueue.pop();
        --demand;
        if(!demand) break;
    }
    //std::cout << "Consumer quit" << std::endl;
    printf("Consumer quit\r\n");
}

int main() {
    std::thread producer1(producer, 0, 10);
    std::thread producer2(producer, 10, 20);
    std::thread producer3(producer, 20, 30);

    std::thread consumer1(consumer, 20);
    std::thread consumer2(consumer, 10);

    producer1.join();
    producer2.join();
    producer3.join();
    consumer1.join();
    consumer2.join();

    return 0;
}