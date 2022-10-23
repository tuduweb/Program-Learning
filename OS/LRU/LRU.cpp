#include <iostream>

class LRUNode{
public:
    int k, v;
    LRUNode *prev, *next;
    LRUNode(int _k, int _v): k(_k), v(_v), prev(nullptr), next(nullptr) {}
};

#include <map>

constexpr int maxSize = 10;

class LRUCache{
public:
    LRUNode *head, *tail;
    std::map<int, LRUNode*> nodesMap;
    int curSize;

    LRUCache(): head(new LRUNode(-1, -1)), tail(head), curSize(1) {}
    void refresh(LRUNode *node) {
        
        node->prev->next = node->next;

        node->next = head;
        head->prev = node;
        node->prev = nullptr;
        head = node;
    }

    int get(int key) {
        if(nodesMap.find(key) == nodesMap.end()) {
            //不存在
            std::cout << key << " not exist" << std::endl;
            return -1; //default -1
        }

        LRUNode *tmp = nodesMap[key];
        refresh(tmp);
        return tmp->v;
    }

    bool put(int key, int value) {
        LRUNode* tmp = nullptr;
        if(nodesMap.find(key) != nodesMap.end()) {
            //already exist
            return false;
        }else if(curSize >= maxSize) {
            tmp = tail;

            tail->prev->next = nullptr;
            tail = tail->prev;

            //delete
            nodesMap.erase(tmp->k);
            // delete tmp;
            // tmp = nullptr;

            //return false;
        }
        if(tmp == nullptr) {
            tmp = new LRUNode(key, value);
            ++curSize;
        }else{
            //clear data of tmp
            tmp->k = key;
            tmp->v = value;
            tmp->prev = nullptr;
        }
        head->prev = tmp;
        tmp->next = head;
        head = tmp;
        nodesMap.insert({key, tmp});

        return true;
    }

    void PrintLRUCache() {
        //打印当前队列情况
        LRUNode *cur = head;
        while(cur) {
            std::cout << cur->k << " ";
            cur = cur->next;
        }
        std::cout << std::endl;
    }

};


int main() {
    LRUCache cache;
    cache.put(1, 2);
    for(int i = 0; i < 10; ++i) {
        cache.put(i + 3, 0);
    }
    cache.PrintLRUCache();

    cache.get(5);

    cache.PrintLRUCache();
    return 0;
}

/**
 * ref: https://blog.csdn.net/qq_46517733/article/details/123092629
*/