#include <iostream>
#include <list>
using namespace std;

class Robot{
public:
    virtual void eat() = 0;
    virtual void sleep() = 0;
};

class Douya{
public:
    void eat() {
        cout << "豆芽吃饭" << endl;
    }

    void sleep() {
        cout << "豆芽睡觉" << endl;
    }
};

//适配器类
class DouyaAdapter: public Robot, public Douya{
public:
    void eat() {
        cout << "机器人模仿:";
        Douya::eat();
    }

    void sleep() {
        cout << "机器人模仿:";
        Douya::sleep();
    }
};

int main() {
    Robot* robot = static_cast<Robot*>(new DouyaAdapter);
    robot->eat();
    robot->sleep();
    delete robot;
    return 0;
}