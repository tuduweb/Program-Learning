#include <iostream>
#include <vector>
#include <string>

//Trie 字典树
class DictTree{

public:
    bool isEnd;
    std::vector<DictTree*> childrens;

public:
    DictTree(): isEnd(false) {
        childrens.assign(26 + 1, nullptr);
    }

    DictTree(char c): isEnd(false) {
        childrens.assign(26 + 1, nullptr);
        childrens[c] = new DictTree();
    }

    DictTree* insert(const std::string& s) {
        DictTree* cur = this;
        //DictTree** next = nullptr;
        for(auto c : s) {
            // // check c
            // next = &cur->childrens[c - 'a'];
            // if(*next == nullptr) {
            //     *next = new DictTree(c);
            // }
            // cur = *next;
            if(cur->childrens[c - 'a'] == nullptr) {
                cur->childrens[c - 'a'] = new DictTree; // in c, has
            }
            cur = cur->childrens[c - 'a'];
        }
        cur->isEnd = true;
        return this;
    }

    bool search(const std::string& s) {
        DictTree* cur = this;
        for(auto c : s) {
            if(cur->childrens[c - 'a'] != nullptr) {
                cur = cur->childrens[c - 'a'];
            }else{
                return false;
            }
        }
        return cur->isEnd;
    }

    bool startsWith(const std::string& prefix) {
        DictTree* cur = this;
        for(auto c : prefix) {
            if(cur->childrens[c - 'a'] != nullptr) {
                cur = cur->childrens[c - 'a'];
            }else{
                return false;
            }
        }
        return true;
    }

    void printAll(DictTree* tree) {
        for(int i = 0; i < 26; ++i) {
            if(tree->childrens[i] != nullptr) {
                std::cout << static_cast<char>(i + 'a') << std::endl;
                printAll(tree->childrens[i]);
            }
        }
    }

    void printAll() {
        printAll(this);
    }

};


int main() {
    DictTree root;
    root.insert("ac");
    root.insert("ab");
    root.insert("abc");
    root.printAll();

    std::cout << root.search("a") << std::endl;
    return 0;
}