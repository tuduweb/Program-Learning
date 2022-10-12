#include <iostream>
#include <string>
using namespace std;

class Pancake{
public:
    string description = "Basic Pancake";
    virtual string getDescription() { return description; }
    virtual double getCost() = 0;
};

//装饰器基类
class CondimentDecorator: public Pancake{
public:
    virtual string getDescription() = 0;
};

//肉煎饼
class MeatPancake: public Pancake{
public:
    MeatPancake() { description = "MeatPancake"; }
    double getCost() { return 6; }
};

//鸡蛋煎饼
class EggPancake: public Pancake{
public:
    EggPancake() { description = "EggPancake"; }
    double getCost() { return 5; }
};


//额外加鸡蛋
class EggDecorator: public CondimentDecorator{
public:
    Pancake* baseCake;
    EggDecorator(Pancake* _baseCake): baseCake(_baseCake) {}
    string getDescription() { return baseCake->getDescription() + ", Egg"; }
    double getCost() { return baseCake->getCost() + 1.5; }
};

//额外加土豆
class PotatoDecorator: public CondimentDecorator{
public:
    Pancake* baseCake;
    PotatoDecorator(Pancake* _baseCake): baseCake(_baseCake) {}
    string getDescription() { return baseCake->getDescription() + ", Potato"; }
    double getCost() { return baseCake->getCost() + 1; }
};

//额外加培根
class BaconDecorator: public CondimentDecorator{
public:
    Pancake* baseCake;
    BaconDecorator(Pancake* _baseCake): baseCake(_baseCake) {}
    string getDescription() { return baseCake->getDescription() + ", Bacon"; }
    double getCost() { return baseCake->getCost() + 2; }
};

int main() {
    Pancake* pan = new EggPancake();
    pan = new PotatoDecorator(pan);
    pan = new BaconDecorator(pan);
    std::cout << pan->getDescription() << " $ : " << pan->getCost() << std::endl;
    
    Pancake* pan2 = new PotatoDecorator(new BaconDecorator( new MeatPancake));
    std::cout << pan2->getDescription() << " $ : " << pan2->getCost() << std::endl;
    
    return 0;
}