#include <iostream>
#include <thread>
#include <vector>
#include <chrono>


int g_total = 0;

void clickHandler(int id) {
    for(int i = 0; i < 10000000; ++i) {
        g_total++;
    }
}

#include <atomic>
std::atomic<int> g_atomic_total(0);
void clickHandlerAtomic(int id) {
    for(int i = 0; i < 10000000; ++i) {
        g_atomic_total++;
    }
}

#include <mutex>
std::mutex g_mutex;
void clickHandleMutex(int id) {
    for(int i = 0; i < 10000000; ++i) {
        std::lock_guard<std::mutex> lock(g_mutex);
        //g_mutex.lock();
        g_total++;
        //g_mutex.unlock();
    }
}


int main() {
    std::vector<std::thread> threads;
    for(int i = 0; i < 3; ++i) {
        threads.push_back(std::thread(clickHandleMutex, i)); //clickHandler //clickHandlerAtomic
    }
    auto begin = std::chrono::high_resolution_clock::now();
    for(auto& th: threads) th.join(); //阻塞等待.
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "value: " << g_total << std::endl; // g_total // g_atomic_total
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() * 1e-6 << "s" << std::endl; //秒计时
    // normal 0.15218s
    // atomic 0.795508s
    // mutex: lock&&unlock 3.68s
    return 0;
}

/**
 * 原子操作的实现跟普通数据类型类似，但是它能够在保证结果正确的前提下，提供比mutex等锁机制更好的性能。
 * 开发过程中，对于多线程的情况下，单个基础数据类型的数据共享安全，尽量使用原子操作代替锁机制。
 * 当需要对代码块进行数据安全保护的时候，就需要选择使用锁机制了。
 */

