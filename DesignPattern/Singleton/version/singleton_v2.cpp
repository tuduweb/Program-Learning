#include <iostream>
#include <mutex>
#include <memory>

class Singleton{
public:
    typedef std::shared_ptr<Singleton> SingletonSharedPtr;
private:
    Singleton() {
        std::cout << "constructor called!" << std::endl;
    }
    Singleton(Singleton&) = delete; //禁止赋值
    Singleton& operator=(const Singleton&) = delete; //禁止拷贝构造
    
    static SingletonSharedPtr m_instance;
    static std::mutex m_mutex;

public:
    ~Singleton() {
        std::cout << "destructor called" << std::endl;
    }

    static SingletonSharedPtr getInstance() {
        if(m_instance == nullptr) {
            std::lock_guard<std::mutex> lk(m_mutex);
            if(m_instance == nullptr) {
                //m_instance = new Singleton;
                m_instance = std::shared_ptr<Singleton>(new Singleton);
            }
        }
        return m_instance;
    }

    //刚才上一行new的部分注释掉，一样可以调用，因为this没有被用到。
    void use() const { std::cout << "in use" << std::endl; }

};

//init static variables out of class
Singleton::SingletonSharedPtr Singleton::m_instance = nullptr;
std::mutex Singleton::m_mutex;

int main() {
    
    Singleton::SingletonSharedPtr instance = Singleton::getInstance();
    Singleton::SingletonSharedPtr instance2 = Singleton::getInstance();
    
    instance->use();

    return 0;
}

/**
 * shared_ptr和mutex都是C++11的标准，以上这种方法的优点是
 * 基于 shared_ptr, 用了C++比较倡导的 RAII思想，用对象管理资源,当 shared_ptr 析构的时候，new 出来的对象也会被 delete掉。以此避免内存泄漏。
 * 加了锁，使用互斥量来达到线程安全。这里使用了两个 if判断语句的技术称为双检锁；好处是，只有判断指针为空的时候才加锁，避免每次调用 get_instance的方法都加锁，锁的开销毕竟还是有点大的。
 * 
 * 不足之处在于
 * 使用智能指针会要求用户也得使用智能指针，非必要不应该提出这种约束; 使用锁也有开销; 同时代码量也增多了，实现上我们希望越简单越好。
 * 更严重的问题，在某些平台，双检验锁会失效。
 * 过程: 使用了同一个pointer memory
 * Step 1. Allocate memory to hold a Singleton object.
 * Step 2. Construct a Singleton object in the allocated memory.
 * Step 3. Make pInstance point to the allocated memory.
*/