#include <iostream>
#include <thread>

class Animal{
private:
    std::string m_name;
public:
    Animal(const std::string& name): m_name(name) {
        std::cout << "new Animal class, name: " << m_name << std::endl;
    }

    void SelfIntroduce(int age) {
        std::cout << "my truth name is " << m_name << " , age " << age << std::endl;
    }

    static void Laugh() {
        std::cout << "hello my name is animal" << std::endl;
    }
};

int main() {
    std::thread thread1(&Animal::Laugh);
    Animal cat("cat");

    //对对象的引用, 要使用std::ref包装
    std::thread thread2(&Animal::SelfIntroduce, std::ref(cat), 23);
    thread1.join();
    std::cout << "thread1 out" << std::endl;
    thread2.join();
    return 0;
}