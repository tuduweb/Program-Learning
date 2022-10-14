#include <iostream>

template<class Key, class Value>
class HashNode
{
public:
    Key _key;
    Value _value;
    HashNode *next;

public:
    HashNode(Key key, Value value): _key(key), _value(value), next(nullptr) {}
    ~HashNode() {}

    HashNode& operator=(const HashNode& node)
    {
        _key = node._key;
        _value = node._value;
        next = node.next;
        return *this;
    }
};

template<class Key, class Value, class HashFunc, class EqualKey>
class HashMap
{
public:
    HashMap(int size);
    ~HashMap();

    bool insert(const Key& key, const Value& value);
    bool del(const Key& key);
    Value& find(const Key& key);
    Value& operator [](const Key& key);

    //addiction functions
    void PrintMap() {
    for(int i = 0; i < _size; ++i) {
        HashNode<Key, Value> *cur = this->table[i];
        std::cout << "==================================" << std::endl;
        std::cout << "Hash Table " << i << std::endl;
        while(cur) {
            HashNode<Key, Value> *tmp = cur;
            cur = cur->next;
            std::cout << tmp->_key << std::endl;
        }
    }
    }

private:
    HashFunc hash;
    EqualKey equal;
    HashNode<Key, Value> **table;
    unsigned int _size;
    Value ValueNULL;
};

template<class Key, class Value, class HashFunc, class EqualKey>
HashMap<Key, Value, HashFunc, EqualKey>::HashMap(int size): _size(size) {
    this->hash = HashFunc();
    this->equal = EqualKey();
    
    table = new HashNode<Key, Value>*[_size];
    for(int i = 0; i < _size; ++i) {
        table[i] = nullptr;
    }
}

template<class Key, class Value, class HashFunc, class EqualKey>
HashMap<Key, Value, HashFunc, EqualKey>::~HashMap() {
    for(int i = 0; i < _size; ++i) {
        HashNode<Key, Value> *cur = this->table[i];
        while(cur) {
            HashNode<Key, Value> *tmp = cur;
            cur = cur->next;
            delete tmp;
        }
    }
    delete table;
}

/**
 * 如果两个元素都映射到了同一索引下表 的位置，这一现象叫做哈希碰撞。
 * 解决办法：1.拉成链式结构；2.找下一个空位，前提哈希表还有位置。
 * 
 * 链表的查找时间复杂度为O(n), 红黑树的查找时间复杂度为O(logn)。因此当挂载节点较多时，采用红黑树的设计会降低时间复杂度。
*/
template<class Key, class Value, class HashFunc, class EqualKey>
bool HashMap<Key, Value, HashFunc, EqualKey>::insert(const Key& key, const Value& value) {
    
    int index = this->hash(key) % _size;
    
    HashNode<Key, Value> *node = new HashNode<Key, Value>(key, value);
    //insert to head
    node->next = table[index];
    table[index] = node;
    
    return true;
}

template<class Key, class Value, class HashFunc, class EqualKey>
bool HashMap<Key, Value, HashFunc, EqualKey>::del(const Key& deletedKey) {
    int index = this->hash(deletedKey) % _size;
    
    HashNode<Key, Value> *node = table[index];
    HashNode<Key, Value> *prev = nullptr;

    while(node) {
        if(deletedKey == node->_key) {

            if(nullptr == prev) {
                table[index] = node->next;
            } else {
                prev->next = node->next;
            }

            delete node;
            return true;

        }
        prev = node;
        node = node->next;
    }

    return false;
}

template<class Key, class Value, class HashFunc, class EqualKey>
Value& HashMap<Key, Value, HashFunc, EqualKey>::find(const Key& key) {
    
    int index = this->hash(key) % _size;

    if(nullptr == table[index]) {
        return ValueNULL;
    } else {
        HashNode<Key, Value> *node = table[index];
        while(node) {
            if(key == node->_key)
                return node->_value;
            
            node = node->next;
        }
    }

    return ValueNULL;
}

template<class Key, class Value, class HashFunc, class EqualKey>
Value& HashMap<Key, Value, HashFunc, EqualKey>::operator[](const Key& key) {
    return find(key);
}


/* 测试代码 */
#include <string>
//hash
class CalHashFunc
{
public:
    int operator()(const std::string& key) {
        int hash = 0;
        for(int i = 0; i < key.length(); ++i) {
            hash = hash << 7 ^ key[i];
        }
        return (hash & 0x7FFFFFFF);
    }
};

//equal
class EqualKeyFunc
{
public:
    bool operator()(const std::string& A, const std::string& B) {
        return A.compare(B) == 0;
    }
};

//测试用例
int main() {
    HashMap<std::string, std::string, CalHashFunc, EqualKeyFunc> hashmap(100);

    hashmap.insert("hello", "world");
    hashmap.insert("why", "dream");
    hashmap.insert("c++", "good");
    hashmap.insert("welcome", "haha");

    using namespace std;

    cout << "after insert:" << endl;
    cout << hashmap.find("welcome").c_str() << endl;
    cout << hashmap.find("c++").c_str() << endl;
    cout << hashmap["why"].c_str() << endl;
    cout << hashmap["hello"].c_str() << endl;

    if (hashmap.del("hello"))
        cout << "remove is ok" << endl;    //remove is ok
    cout << hashmap.find("hello").c_str() << endl; //not exist print NULL

    hashmap["why"] = "love";
    cout << hashmap["why"].c_str() << endl;

    hashmap.PrintMap();

    return 0;

}


/**
 * Q: HashMap为什么需要扩容？
 * A: ​为了提高查询效率(如果不扩容，只能任由链表一直增长或者树化)。
 * 当HashMap中的数据足够多的时候，他的查询效率变得很低，甚至退化成O(n)，哪怕是使用红黑树也无法使查询效率有较大幅度提高的时候，也就是HashMap中的碰撞达到了负载因子的时候，就会触发HashMap的扩容；
 * 
 * Q: HashMap数据量变多，提高效率？
 * A: 1.红黑树 2.扩容机制
*/