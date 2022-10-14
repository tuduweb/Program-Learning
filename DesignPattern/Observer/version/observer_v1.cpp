#include <iostream>
#include <list>
#include <string>

namespace Learning{
    class Observer;
    //目标 被观察的目标, 目标中定义观察者集合
    class Subject{
    public:
        virtual int notify() = 0;

        void attach(Observer* ob) {
            obLists.push_back(ob);
        }
        void detach(Observer* ob) {
            obLists.remove(ob);
        }

    protected:
        std::list<Observer*> obLists;
    };

    //观察者
    class Observer{
    public:
        virtual int handle() = 0;
        const std::string& getName() const { return m_name; }
        void setName(const std::string& name) { this->m_name = name; }
    private:
        std::string m_name;
    };

    class ConCenter: public Subject{
    public:
        enum INFO_TYPE{
            NONE,
            RESOURCE,
            HELP,
            MAX
        };
        virtual void notify(INFO_TYPE infoType, std::string name) = 0;
    };

    class Player: public Observer{
    public:
        Player() { setName("None"); }
        Player(const std::string& name) { setName(name); }
        Player(const char* name) { setName(name); }

        int handle() override { return 0; }

        void help() {
            printf("%s:坚持住，我来救你！\n", getName().c_str());
        }

        void come(){
            printf("%s:好的，我来取物资\n", getName().c_str());
        }

        void call(ConCenter::INFO_TYPE infoType, ConCenter* con) {
            switch (infoType){
            case ConCenter::RESOURCE:
                printf("%s :我这里有物资\n", getName().c_str());
                break;
            case ConCenter::HELP:
                printf("%s :救救我\n", getName().c_str());
                break;
            default:
                printf("Nothing\n");
            }
            con->notify(infoType, getName());
        }

    };

    class ConCenterController: public ConCenter{
    public:
        ConCenterController() {}
        void notify(INFO_TYPE infoType, std::string name) override;
        int notify() override { return 0; };
    };

    void ConCenterController::notify(INFO_TYPE infoType, std::string name) {
        switch(infoType) {
            case ConCenter::RESOURCE:
                for(Observer* player: obLists) {
                    if(player->getName() != name) {
                        static_cast<Player*>(player)->come();
                    }
                }
                break;
            case ConCenter::HELP:
                for(Observer* player: obLists) {
                    if(player->getName() != name) {
                        static_cast<Player*>(player)->help();
                    }
                }
                break;
            default:
                printf("nothing happens\n");
        }
    }
};

#include <vector>
#include <unordered_map>

int main() {
    std::unordered_map<std::string, Learning::Player*> playerMap;
    std::vector<Learning::Player*> players;
    Learning::ConCenterController* controller = new Learning::ConCenterController;
    
    // players.emplace_back(new Learning::Player("打野"));
    // players.emplace_back(new Learning::Player("歌手"));
    // players.emplace_back(new Learning::Player("傻子"));
    // players.emplace_back(new Learning::Player("人才"));
    // players.emplace_back(new Learning::Player("宝"));
    // // players.emplace_back("傻子");
    // // players.emplace_back("人才");

    auto addPlayer = [&playerMap, &players](const std::string& name) -> bool {
        Learning::Player* _player = new Learning::Player(name);
        players.emplace_back(_player);
        playerMap.insert({name, _player});
        return true;
    };

    addPlayer("打野");
    addPlayer("上单");
    addPlayer("下路");
    addPlayer("游走");
    addPlayer("辅助");
    addPlayer("中单");

    for(auto player: players) {
        std::cout << player->getName() << std::endl;
        controller->attach(player);
    }

    playerMap["打野"]->call(Learning::ConCenter::HELP, controller);
    playerMap["下路"]->call(Learning::ConCenter::RESOURCE, controller);

    return 0;
}