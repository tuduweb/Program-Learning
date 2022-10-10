#include <iostream>

class Singleton{
private:
    Singleton() {
        std::cout << "constructor called!" << std::endl;
    }
    Singleton(Singleton&) = delete; //禁止赋值
    Singleton& operator=(const Singleton&) = delete; //禁止拷贝构造
    
    static Singleton* m_instance;

public:
    ~Singleton() {
        std::cout << "destructor called" << std::endl;
    }

    static Singleton* getInstance() {
        if(m_instance == nullptr) {
            m_instance = new Singleton;
        }
        return m_instance;
    }

    void use() const { std::cout << "in use" << std::endl; }
};

Singleton* Singleton::m_instance = nullptr;

int main() {
    
    Singleton* instance = Singleton::getInstance();
    Singleton* instance2 = Singleton::getInstance();

    return 0;
}

/**
 * 存在的问题
 * 线程安全问题
 * 内存泄漏（没有负责delete） 解决办法，使用共享指针
*/