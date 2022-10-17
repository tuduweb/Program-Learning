#include <iostream>
#include <queue>

constexpr int DEGREE_NUM = 2;
constexpr int KEYMAX_NUM = DEGREE_NUM * 2;
constexpr int KEYMIN_NUM = DEGREE_NUM;
constexpr int KEYNUM_MAX = KEYMAX_NUM;
constexpr int KEYNUM_MIN = KEYMIN_NUM;
constexpr int CHILD_MAX = KEYMAX_NUM + 1;

/**
 * BPTNode 基本实现, 作为结点的基类
 * BPTInterNode 内部结点
 * BPTLeafNode 叶子结点
 * 内结点存有关键字和孩子结点的指针，叶子结点存有关键字和数据
 * 
 * 分裂操作, 针对于叶子结点和内部结点两种情况..
 */
template <class T, class DataT>
class BPTInterNode;

template<class T, class DataT>
class BPTree;

template <class T, class DataT>
class BPTLeafNode;


template <class T, class DataT>
class BPTNode{
    friend class BPTree<T, DataT>;
    friend class BPTLeafNode<T, DataT>;
    friend class BPTInterNode<T, DataT>;

protected:
    typedef BPTNode<T, DataT> Node;
    bool isLeaf;
    BPTInterNode<T, DataT> *parent;
    
    T keyDatas[DEGREE_NUM * 2];
    int keyNum; //关键字个数

    //查找本结点的兄弟结点
    Node* GetBrother(int& isLeftFlag) {
        if(parent == nullptr) return nullptr;

        Node *siblingNode;
        for(int i = 0; i < parent->keyNum + 1; i++) {
            
            if(parent->childrens[i] == this) {
                if(parent->keyNum == i) {
                    siblingNode = parent->childrens[i - 1];
                    isLeftFlag = 1;
                }else{
                    siblingNode = parent->childrens[i + 1];
                    isLeftFlag = 0;
                }
            }

        }
        return siblingNode;
    }
public:
    BPTNode(): isLeaf(true), keyNum(0), parent(nullptr) {}
    void PrintAll() {
        for(int i = 0; i < this->keyNum; ++i) {
            std::cout << this->keyDatas[i] << " ";
        }
        std::cout << std::endl;
    }
};

template<class T, class DataT>
class BPTInterNode: public BPTNode<T, DataT>{
    friend class BPTree<T, DataT>;
private:
    BPTNode<T, DataT>* childrens[CHILD_MAX]; //查找路径

    /**
     * 将this分裂成两个块
     * @param newNode 新节点, 接受老节点部分数据
     * @param key 分裂值
    */
    T __splitBlock(BPTInterNode<T, DataT> *newNode, T key) {

        //int i = 0, j = 0;
        //判断分裂点位置.. 当前 DEGREE_NUM * 2 + 1, 会拆成奇+偶结构
        // 0 1 2 4 -> 0 1| 2 3 这里会形成对等分割
        if(key > this->keyDatas[DEGREE_NUM - 1] && key < this->keyDatas[DEGREE_NUM]) {
            for(int i = DEGREE_NUM; i < KEYMAX_NUM; ++i) {
                newNode->keyDatas[i - DEGREE_NUM] = this->keyDatas[i];
                newNode->childrens[i - DEGREE_NUM] = this->childrens[i]; //childI = keyI + 1
                newNode->childrens[i - DEGREE_NUM]->parent = newNode;
            }
            // newNode->childrens[DEGREE_NUM + 1] = this->childrens[KEYNUM_MAX];
            // newNode->childrens[DEGREE_NUM + 1]->parent = newNode;
            this->keyNum = DEGREE_NUM;
            newNode->keyNum = KEYNUM_MAX - DEGREE_NUM;
            return key; //key比原node都大, 比新node都小
        }

        //reach here, only < [D - 1] or > [D]
        //分裂位置 形成不对等分割
        int insertPos = key < this->keyDatas[KEYMIN_NUM - 1] ? KEYMIN_NUM - 1: KEYMIN_NUM;
        //分裂位置的值, 分为最大值和最小值.. insertPos为最大值, insertPos+1为最小值
        T splitValue = this->keyDatas[insertPos + 1];
        //分裂位置后的值, 拷贝到新块中
        // 0 1 2
        //0 1 2 3
        for(int i = insertPos + 1, j = 0; i < KEYMAX_NUM; ++i, ++j) {
            newNode->keyDatas[j] = this->keyDatas[i];
            newNode->childrens[j] = this->childrens[i];
            newNode->childrens[j]->parent = newNode;
        }
        //最后一个位置的挪动, 理论上B+树最后一个位置?
        //newNode->childrens[KEYMAX_NUM - insertPos] = this->childrens[KEYMAX_NUM];
        //newNode->childrens[KEYMAX_NUM - insertPos]->parent = newNode;
        this->keyNum = insertPos + 1;
        newNode->keyNum = KEYMAX_NUM - insertPos - 1; //满足两个keyNum之和为KEYMAX_NUM

        //该值会插入到父结点中
        return splitValue;
    }

public:
    bool __insert(BPTNode<T, DataT> *newNode, T data) {
        int i = 0;
        for(; i < this->keyNum && data > this->keyDatas[i]; ++i);
        //执行挪位置
        for(int j = this->keyNum; j > i; --j) {
            this->keyDatas[j] = this->keyDatas[j - 1];
            this->childrens[j] = this->childrens[j -1];
        }
        ++this->keyNum;
        this->keyDatas[i] = data;
        this->childrens[i] = newNode;
        newNode->parent = this;
        return true;
    }

public:
    BPTInterNode() {
        this->isLeaf = false;
        for(int i = 0; i < DEGREE_NUM * 2 + 1; ++i)
            childrens[i] = nullptr;
    }
};

template<class T, class DataT>
class BPTLeafNode: public BPTNode<T, DataT>{
    friend class BPTree<T, DataT>;
private:
    //含有实际数据
    DataT data;

    //链式结构
    BPTLeafNode<T, DataT>* prev;
    BPTLeafNode<T, DataT>* next;

    using BPTNode<T, DataT>::keyNum;
    using BPTNode<T, DataT>::keyDatas;
    using BPTNode<T, DataT>::isLeaf;
    using BPTNode<T, DataT>::parent;

public:
    BPTLeafNode(): prev(nullptr), next(nullptr) {
        this->isLeaf = true;
    }
    
    //BPTLeafNode<T, DataT>* __insert(int value) {
    bool __insert(int value) {
        int i = 0;
        //找合适位置, 插入到位置i
        for(; i < keyNum && value > keyDatas[i]; ++i) {}
        //移动, 腾出位置
        for(int j = keyNum; j > i; --j) {
            keyDatas[j] = keyDatas[j - 1];
        }
        //插入操作.
        keyDatas[i] = value;
        ++keyNum;

        return true;
    }

    bool __delete(int value) {
        bool found = false;
        int i = 0;
        for(;i < keyNum && value >= keyDatas[i]; ++i) {}
        if(i < keyNum && value == keyDatas[i]) {
            found = true;
        }
        if(!found) return false;

        //删除..
        for(int j = i; j < keyNum - 1; j++) {
            keyDatas[j] = keyDatas[j + 1];
        }
        --keyNum;
        return true;
    }

    void __splitBlock(BPTNode<T, DataT> *nodeParent, int childIndex, BPTLeafNode<T, DataT> *nodeChild) {
        for(int i = 0; i < DEGREE_NUM; ++i) {
            nodeChild->keyDatas[i] = this->keyDatas[i + DEGREE_NUM];
        }
        this->keyNum = this->keyNum - DEGREE_NUM;
        nodeChild->keyNum = DEGREE_NUM;
    }

    T __splitBlock(BPTLeafNode<T, DataT> *newNode) {
        for(int i = 0; i < DEGREE_NUM; ++i) {
            newNode->keyDatas[i] = this->keyDatas[i + DEGREE_NUM];
        }
        this->keyNum = this->keyNum - DEGREE_NUM;
        newNode->keyNum = DEGREE_NUM;

        if(this->next != nullptr)
            this->next->prev = newNode;

        newNode->next = this->next;
        this->next = newNode;
        newNode->prev = this;

        //下一块中, 最小的元素..
        return newNode->keyDatas[0];
    }

    /**
     * 合并块
     * 什么情况下要合并? 当删除后数目小于一定数量的时候, 就需要合并了..
     * 1 10 12  | 33 36  | 39 42  | 45 48  | 51 54  | 57 60  | (end) 比如查找57 会掉入 57,60中
     */
    void __deleteData(T data) {
            //
    }
    void __merge(BPTLeafNode<T, DataT> *oldNode) {
        //
    }
};

template<class T, class DataT>
class BPTree{
    typedef int data_type;
private:
    BPTNode<T, DataT> *treeRoot;
public:
    BPTree() { return; }

    //找data理论上应该存在的LeafNode
    BPTLeafNode<T, DataT>* Find(T data) {

        BPTNode<T, DataT> *cur = treeRoot;
        //BPTInterNode<T> *interNode;
        int i = 0;

        while(cur != nullptr) {
            if(cur->isLeaf) break;
            for(; i < cur->keyNum && data >= cur->keyDatas[i]; ++i) {};
            if(i > 0) i = i - 1; //处理边界
            //interNode = cur;
            BPTInterNode<T, DataT> *tmp = static_cast<BPTInterNode<T, DataT> *>(cur);
            cur = static_cast<BPTInterNode<T, DataT> *>(cur)->childrens[i];
            //这里需要增加对边界的处理, 这里有越界的可能!
        }
        
        return static_cast<BPTLeafNode<T, DataT> *>(cur);
    }

    bool Search(T data) {
        BPTLeafNode<T, DataT> *leafNode = nullptr;
        leafNode = Find(data);
        if(leafNode == nullptr) return false;

        int i = 0;
        for(; i < leafNode->keyNum; ++i) {
            if(data == leafNode->keyDatas[i])
                break;
        }

        return i < leafNode->keyNum;
    }

    /**
     * 向node中, 插入一个新节点, 其中分割值为extreameData, 分割后的节点为nodeToInsert
    */
    bool AddNode(BPTInterNode<T, DataT> *node, T extremeData, BPTNode<T, DataT> *nodeToInsert) {
        if(node == nullptr || node->isLeaf) return false;
        
        if(node->keyNum < KEYMAX_NUM) {
            return node->__insert(nodeToInsert, extremeData);
        }

        /* 以下为需要分割后, 再插入的情况 */
        BPTInterNode<T, DataT> *newInterNode = new BPTInterNode<T, DataT>;
        //分割成小于extreme, 大于extreme的情况
        T newData = node->__splitBlock(newInterNode, extremeData);
        //判断需要插入到左边还是右边
        if(node->keyNum <= newInterNode->keyNum)
            node->__insert(nodeToInsert, extremeData);
        else if(node->keyNum > newInterNode->keyNum)
            newInterNode->__insert(nodeToInsert, extremeData);
        else{
            //需要跟split结合调试, 使用
            newInterNode->childrens[0] = nodeToInsert;
            nodeToInsert->parent = newInterNode;
        }

        BPTInterNode<T, DataT> *parentNode = node->parent;
        if(nullptr == parentNode) {
            parentNode = new BPTInterNode<T, DataT>;
            parentNode->childrens[0] = node;
            parentNode->childrens[1] = newInterNode;
            parentNode->keyDatas[0] = node->keyDatas[0]; //分裂之后的回传值, 作为插入的中间值..
            parentNode->keyNum = 1;
            node->parent = parentNode;
            treeRoot = parentNode;
            //return true;
        }

        //这里是对Inter实行了分割, 需要继续操作上行节点, 以完成整体结构
        return AddNode(parentNode, newData, newInterNode);
    }

    bool Insert(T data) {
        //可能要插入的根节点位置..
        BPTLeafNode<T, DataT> *oldLeafNode = Find(data);

        if(oldLeafNode == nullptr) treeRoot = oldLeafNode = new BPTLeafNode<T, DataT>;

        int extremePos = 0; //为搜索到的临界位置, 满足 keyDatas[extremePos - 1] < data <= ..
        for(; extremePos < oldLeafNode->keyNum && data < oldLeafNode->keyDatas[extremePos]; ++extremePos);
        if(extremePos < oldLeafNode->keyNum && data == oldLeafNode->keyDatas[extremePos]) return false;

        if(oldLeafNode->keyNum < KEYMAX_NUM) {
            bool res = oldLeafNode->__insert(data);
            if(data == oldLeafNode->keyDatas[0]) {
                //需要特殊处理
                BPTInterNode<T, DataT> *interNode = oldLeafNode->parent;
                BPTNode<T, DataT> *childNode = oldLeafNode;
                
                //更新上层..
                while(interNode) {
                    int i = 0;
                    for(; i < interNode->keyNum && interNode->childrens[i] != childNode; ++i){};
                    interNode->keyDatas[i] = data;
                    childNode = interNode;
                    interNode = interNode->parent;
                }
            }
            return res;
        }
        
        //如果在某结点的数据, 达到了一定大小, 那么是无法插入的, 需要分裂结点
        BPTLeafNode<T, DataT> *newLeafNode = new BPTLeafNode<T, DataT>;

        //在split里面改变了链表关系, 这里可能会出问题..
        T extremeDataInNewNode = oldLeafNode->__splitBlock(newLeafNode);

        //判断是要插入old, 还是new中.. extremeDataInNode为newLeafNode首值..
        if(data < extremeDataInNewNode) {
            oldLeafNode->__insert(data);
    
            // // 这里不会出现边界情况, 因为出现这种需要送到0位置的时候, 其实被送到左兄弟上了
            // if(data == oldLeafNode->keyDatas[0]) {
            //     //需要特殊处理
            //     BPTInterNode<T, DataT> *interNode = oldLeafNode->parent;
            //     BPTNode<T, DataT> *childNode = oldLeafNode;
                
            //     //更新上层..
            //     while(interNode) {
            //         int i = 0;
            //         for(; i < interNode->keyNum && interNode->childrens[i] != childNode; ++i){};
            //         interNode->keyDatas[i] = data;
            //         childNode = interNode;
            //         interNode = interNode->parent;
            //     }
            // }

        }
        else
            newLeafNode->__insert(data);
        
        //extremeDataInNewNode = newLeafNode->keyDatas[0];//这里可能没必要

        //更改父亲
        BPTInterNode<T, DataT> *parentInterNode = oldLeafNode->parent;
        if(parentInterNode == nullptr) {
            //没有父结点的情况, 那么需要新建一个
            parentInterNode = new BPTInterNode<T, DataT>;
            parentInterNode->childrens[0] = oldLeafNode;
            parentInterNode->childrens[1] = newLeafNode;
            parentInterNode->keyNum = 1;
            parentInterNode->keyDatas[0] = oldLeafNode->keyDatas[0];
            
            oldLeafNode->parent = parentInterNode;
            newLeafNode->parent = parentInterNode;

            treeRoot = parentInterNode;
            //return true;
        }

        //向父结点插入新建的分割块, 理论上其值为新拆出块<newLeafNode>的首值
        //在这里已经完成了newLeafNode的建立, 和value的插入。所以一定可以获取到这个"首值"
        //TODO: 分情况讨论，完成代码修改
        bool res;

        res = AddNode(parentInterNode, extremeDataInNewNode, newLeafNode);
        return res;
        //return parentInterNode->
        //parentInterNode->__insert()
    }

    bool Remove(T data) {
        BPTLeafNode<T, DataT> *oldLeafNode = Find(data);
        if(oldLeafNode == nullptr) return false;
        oldLeafNode->PrintAll();
        //TODO: remove
        return true;
    }


    void PrintAll() {
        using namespace std;
        BPTNode<T, DataT> *cur = treeRoot;
        if(cur == nullptr) {
            std::cout << "tree is empty" << std::endl;
        }
        while(cur->isLeaf == false) {
            cur->PrintAll();
            cur = static_cast<BPTInterNode<T, DataT> *>(cur)->childrens[0];
        }

        BPTLeafNode<T, DataT> *leafNode = static_cast<BPTLeafNode<T, DataT> *>(cur);
        while(leafNode) {
            for(int i = 0; i < leafNode->keyNum; ++i) {
                std::cout << leafNode->keyDatas[i] << " ";
            }
            std::cout << " | ";
            leafNode = leafNode->next;
        }

        std::cout << "(end)" << std::endl;
    }

    void PrintTree() {
        BPTNode<T, DataT> *node = treeRoot;
        if(node == nullptr) return;
        int height = 0;
        std::queue<BPTNode<T, DataT>*> nodeQueue;
        std::queue<int> nodeHeightQueue; //subQueue of node Queue
        nodeQueue.push(node);
        nodeHeightQueue.push(1);

        while(!nodeQueue.empty()) {
            node = nodeQueue.front(); //also top

            if(height != nodeHeightQueue.front()) {
                std::cout << std::endl;
                std::cout << "height " << height << std::endl;
                height = nodeHeightQueue.front();
            }

            nodeQueue.pop();
            nodeHeightQueue.pop();

            node->PrintAll();
            //遍历
            if(node != nullptr && node->isLeaf == false) {
                BPTInterNode<T, DataT> *interNode = static_cast<BPTInterNode<T, DataT> *>(node);
                for(int i = 0; i < interNode->keyNum; ++i) {
                    nodeQueue.push(interNode->childrens[i]);
                    nodeHeightQueue.push(height + 1);
                }
            }

        }
    }
};


/**
 * 实现
 */
#include <cstring>
int main(int argc, char** argv) {
    BPTree<int, int>* tree = new BPTree<int, int>;
    tree->Insert(10);
    tree->Insert(12);

    int length = 10;
    if(argc == 2) length = std::stoi(argv[1]);
    for(int i = 20; i >= 10 + length; --i) {
        tree->Insert(i * 3);
    }
    
    tree->PrintTree();
    std::cout << std::endl;
    tree->PrintAll();

    std::cout << std::endl;
    tree->Insert(1); //需要更新边界
    //tree->Insert(100);
    tree->PrintTree();
    std::cout << std::endl;
    tree->PrintAll();

    tree->Remove(39);

    return 0;
}

/**
 * 来源：https://blog.csdn.net/weixin_51309915/article/details/122592023
 * 在来源中有部分错误, 似乎和其它地方对B+树的定义也有出入。最明显就是childrens和keyDatas的关系, 是相等还是差1
 * 
 * TODO:
 * 1. 边界条件的处理需要修复
*/