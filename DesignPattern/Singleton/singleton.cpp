#include <iostream>
#include <mutex>

class CommFactory{
public:
    void call() {
        std::cout << "hello in comm factory" << std::endl;
    }
};

template<class T>
class Singlelon {
public:
   static T* getIns() {
       static T* ins = nullptr;
       static std::mutex mutex;
       if (ins == nullptr) {
           mutex.lock();
           if (ins == nullptr) {
               ins = new T;
           }
           mutex.unlock();
       }
       return ins;
   }
};


#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

void f1(int n)
{
    CommFactory* factory = Singlelon<CommFactory>::getIns();
    CommFactory* factory2 = Singlelon<CommFactory>::getIns();

    std::cout << "equal? " << (factory2 == factory) << std::endl;

    for (int i = 0; i < 5; ++i) {
        std::cout << "Thread " << n << " executing\n";
        factory->call();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void f2(int& n)
{
    for (int i = 0; i < 5; ++i) {
        std::cout << "Thread 2 executing\n";
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main()
{
    int n = 0;
    std::thread t1; // t1 is not a thread
    std::thread t2(f1, n + 1); // pass by value
    std::thread t3(f2, std::ref(n)); // pass by reference
    std::thread t4(std::move(t3)); // t4 is now running f2(). t3 is no longer a thread
    t2.join();
    t4.join();
    std::cout << "Final value of n is " << n << '\n';
}

/** Run
 * g++ -o demo singleton.cpp -pthread && ./demo
 */