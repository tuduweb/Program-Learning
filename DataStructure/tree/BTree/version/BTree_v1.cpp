#include <iostream>

//  k0  k1
//c0  c1  c2

//  k0  k1  k3
//c0  c1  c2  c3
constexpr int DEGREE_MIN = 2; //最小度
constexpr int KEYNUM_MAX = DEGREE_MIN * 2 - 1;
//最小数
constexpr int KEYNUM_MIN = DEGREE_MIN - 1;
constexpr int CHILDREN_MAX = KEYNUM_MAX + 1;
constexpr int CHILDREN_MIN = KEYNUM_MIN + 1;

template<class T>
class BTree; //前置声明

template<class T>
class BTNode{
    friend class BTree<T>; //友元
public:
    bool isLeaf; //叶子节点
    BTNode<T> *parent; //指向爹
    BTNode<T> *childrens[CHILDREN_MAX]; //指向儿子们

    int keyNum; //关键字个数
    T keyDatas[KEYNUM_MAX];
public:
    BTNode(): keyNum(0), isLeaf(true), parent(nullptr) {
        for(int i = 0; i < CHILDREN_MAX; ++i)
            childrens[i] = nullptr;
        for(int i = 0; i < KEYNUM_MAX; ++i)
            keyDatas[i] = 0; //这里是不是需要一个初始化的..方法?
    }
    ~BTNode() {
        //析构
    }
};

template<class T>
class BTree{
private:
    BTNode<T> *bTreeRoot;

    BTNode<T>* __search(BTNode<T> *node, T value) {
        if(node == nullptr) return nullptr;
        int i = 0;
        for(i = 0; i < node->keyNum; ++i) {
            if(value <= node->keyDatas[i])
                break;
        }

        if(i < node->keyNum && value == node->keyDatas[i]) {
            //值在当前块的关键数中 返回当前块
            return node;
        }else{
            //当前关键数中找不到.. 那么有两种情况. 1)叶子结点 2)向下继续
            if(node->isLeaf) {
                return nullptr;
            }else{
                //              lastK (当前快的值, 最大为上一层的key[m])
                //   k0  k1 v k2      (当前v <= k[2], 那么要去 p[2]找)
                // p0  p1  p2  p3     (如果v 在 k[2]右边, 这时i=3, 去p[3])
                // 推导:
                // v < k[2] -> p2
                return __search(node->childrens[i], value);
            }
        }

    }

    BTNode<T>* __insert_notFull(BTNode<T> *node, T value) {

        int nodeKeyNum = node->keyNum;

        if(node->isLeaf) { //当前是叶子结点.. 那么就说明现在没儿子指针了.. 在当前插入
            while(nodeKeyNum > 0 && value < node->keyDatas[nodeKeyNum - 1]) {
                //从最大的开始比较
                //如果v比最右边的还大, 判断直接不成立, 直接插入.. 此时下标为keyNum
                //如果v比<比较值>小, 那么需要插入到<比较值>左边; 把比较值右移. 注意首值, 为keyNum
                node->keyDatas[nodeKeyNum] = node->keyDatas[nodeKeyNum - 1];
                //下标移动
                --nodeKeyNum;
            }
            //此时, pos = 0; 或者找到了 value > itemData的位置..
            //上一步腾了位置nodeKeyNum - 1. 而下标已经-1了, 那么当前"空"的就是nodeKeyNum
            node->keyDatas[nodeKeyNum] = value;
            ++node->keyNum;
            //返回插入的位置所属的块
            return node;

        }else{
            //不是叶子结点. 那么可以继续往下找, 直到找到叶子结点, 才最终实施插入
            //找比value所对应的child
            //   k0  k1 v k2      (v > data[k1], nodeKeyNum = 2 此时跳出, 且要去p2找)
            // p0  p1  p2  p3 (思考最次情况, nodeKeyNum = 0, 此时要去 p0找, keyNode为所求)
            while(nodeKeyNum > 0 && value < node->keyDatas[nodeKeyNum - 1]) {
                --nodeKeyNum;
            }
            //这时候, 找到的下标为需要往下找的儿子
            BTNode<T> *child = node->childrens[nodeKeyNum];
            //如果这个儿子满了, 那么拆儿子; 并且, 当前已经保证, 当前块notFull
            if(KEYNUM_MAX == child->keyNum) {
                //想加入的地方满了, 那么需要把这个儿子块分裂, 并把儿子块中间的数提到上面来..
                //此时, 块中所有元素 小于 node->keyDatas[nodeKeyNum]
                //所以, 在分裂时, 需要从儿子里面提取一个关键数字, 到nodeKeyNum位置..
                // parent = node(当前), 这个儿子在爸爸这的下标为arg1, 儿子已经赋值为child
                __splitBlock(node, nodeKeyNum, child);
                
                //判断是否需要修正插入位置
                //因为把儿子给拆成了两部分, 所以需要和从儿子提上来的<关建数>比较大小, 更新儿子指针位置
                //提升前, value 比keyDatas[nodeKeyNum] 小
                //提升后, value 不一定小了, 比较一下, 如果比这个值大了, 那么往后推一位
                //p0 -> p1(pm -> pm+1)
                if(value > node->keyDatas[nodeKeyNum]) {
                    child = node->childrens[nodeKeyNum + 1];
                }
            }
            //判断结束, 修正完成; 正式开始在儿子中找位置插入..
            return __insert_notFull(child, value);
        }

        //never reach
        return nullptr;
    }

    BTNode<T>* __splitBlock(BTNode<T> *nodeParent, int childIndex, BTNode<T> *nodeChild) {
        //新建一个块, 满块的右边需要移动到新块中
        BTNode<T> *nodeRight = new BTNode<T>();
        //跟需要分裂的一致..
        nodeRight->isLeaf = nodeChild->isLeaf;
        nodeRight->keyNum = KEYNUM_MIN;

        //
        //int i = 0;
        for(int i = 0; i < KEYNUM_MIN; ++i) {
            //将满的, 右边的拷贝过来.. //需要重新计算
            //degree = 2, keymin = 1, keymax = 3; 0 -> 2
            //k0 k1 k2
            //degree = 3, keymin = 2, keymax = 5; 0 -> 3, 1 -> 4
            //k0 k1 k2 k3 k4
            nodeRight->keyDatas[i] = nodeChild->keyDatas[KEYNUM_MIN + 1 + i];
        }

        if(!nodeChild->isLeaf) { //不是叶子节点, 那么需要处理childrens
            //
            for(int i = 0; i < CHILDREN_MIN; ++i) {
                // nodeChild   | nodeRight
                // 0 1         | 2 3
                //             | 0 1 []
                nodeRight->childrens[i] = nodeChild->childrens[CHILDREN_MIN + i];
                //移动的部分, "爸爸"变了
                nodeChild->childrens[CHILDREN_MIN + i]->parent = nodeRight->childrens[i];
            }
        }

        //关键数更新
        //nodeRight->keyNum = KEYNUM_MIN;
        nodeChild->keyNum = KEYNUM_MIN; //nodeChild->keyNum - 

        //处理父节点..
        //将父节点中, 找到的插入位置之后的东西, 全部挪动 childNum = keyNum + 1
        //nodeParent
        for(int i = nodeParent->keyNum; i > childIndex; --i) {
            nodeParent->childrens[i + 1] = nodeParent->childrens[i];
            nodeParent->keyDatas[i] = nodeParent->keyDatas[i - 1];
            //槽位变动
            nodeChild->childrens[i]->parent = nodeParent->childrens[i + 1];
        }
        ++nodeParent->keyNum; //更新keyNum
        //被拆分的childIndex的右边一位, 就需要放置新增的块
        //关键词是中间那一个, 把中间那一个移到上面来
        //childrens[childIndex]对应child, 那么nodeRight就放右边一个
        nodeParent->childrens[childIndex + 1] = nodeRight;
        //空余的位置, 放移出来那个
        //k0 <k1> k2
        nodeParent->keyDatas[childIndex] = nodeChild->keyDatas[KEYNUM_MIN];

        nodeRight->parent = nodeParent->childrens[childIndex + 1]; //爹是这个位置.
        //返回新建的块
        return nodeRight;
    }

    //BTNode<T> *node, 爹可能改变, 所以不能随意设置..
    BTNode<T>* __insert(T value) {
        //数据存在, 不允许插入
        //if(contain(value)) return nullptr;

        //不存在, 则新建 这里是有问题的..
        //TODO: 修改这里
        if(bTreeRoot == nullptr) bTreeRoot = new BTNode<T>();

        //根结点满, 需要特殊处理.
        if(bTreeRoot->keyNum == KEYNUM_MAX) { //达到了最大..
            //新建一个结点
            BTNode<T> *newNode = new BTNode<T>();
            newNode->isLeaf = false; //这个结点 是要保存指针的, 所以不是叶子

            newNode->childrens[0] = bTreeRoot; // 容易理解, 第一个位置肯定是保存当前结点的.. data[k] > data[0] (k>0)
            //拆分当前.. 一分为二. 在处理中, 会提升元素到arg0中..即newNode中..
            __splitBlock(newNode, 0, bTreeRoot);
            //新建的结点变成爹..
            bTreeRoot = newNode;
        }

        //重新处理插入, 这时候full的情况已经被处理, 满足notFull
        return __insert_notFull(bTreeRoot, value);

    }


    void __traversal(BTNode<T> *node) const {
        if(node == nullptr) return;
        for(int i = 0; i < node->keyNum; ++i) {
            __traversal(node->childrens[i]);
            std::cout << node->keyDatas[i] << " ";
        }
        __traversal(node->childrens[node->keyNum]);
    }

    void __deleteNode(T value) {

        if(bTreeRoot->keyNum == 1) {
            if(bTreeRoot->isLeaf) {
                delete bTreeRoot;
                bTreeRoot = nullptr;
            }else{
                //根结点只有一个, 且关键字不为叶子结点..
                BTNode<T> *nodeChild1 = bTreeRoot->childrens[0];
                BTNode<T> *nodeChild2 = bTreeRoot->childrens[1];
                //需要减少一层高度的情况.
                if(nodeChild1->keyNum == KEYNUM_MIN && nodeChild2->keyNum == KEYNUM_MIN) {
                    __mergeBlock(bTreeRoot, 0);
                    //融合后删掉根结点
                    delete bTreeRoot;
                    bTreeRoot = nodeChild1;
                }
            }
        }

        __NodeDeleteThenBalance(bTreeRoot, value);


    }


    BTNode<T>* __getMaxNodeInNode(BTNode<T> *node) {
        //
        while(!node->isLeaf) {
            node = node->childrens[node->keyNum];
        }
        return node;
    }

    BTNode<T>* __getMinNodeInNode(BTNode<T> *node) {
        //
        while(!node->isLeaf) {
            node = node->childrens[0];
        }
        return node;
    }

    T _getMaxInNode(BTNode<T> *node) {
        node = __getMaxNodeInNode(node);
        return node->keyDatas[node->keyNum - 1];
    }

    T _getMinInNode(BTNode<T> *node) {
        node = __getMinNodeInNode(node);
        return node->keyDatas[0];
    }

    /**
     * 合并两个连续的块, 第一个块index为入参
    */
    BTNode<T>* __mergeBlock(BTNode<T> *nodeParent, int childFirstIndex) {
        BTNode<T> *nodeChild1 = nodeParent->childrens[childFirstIndex];
        BTNode<T> *nodeChild2 = nodeParent->childrens[childFirstIndex + 1];

        //nodeChild1 node->keyDatas[childFirstIndex] nodeChild2
        nodeChild1->keyDatas[nodeChild1->keyNum] = nodeParent->keyDatas[childFirstIndex];
        for(int i = 0; i < nodeChild2->keyNum; ++i) {
            nodeChild1->keyDatas[nodeChild1->keyNum + 1 + i] = nodeChild2->keyDatas[i];
        }
        if(!nodeChild2->isLeaf) {
            for(int i = 0; i < nodeChild2->keyNum + 1; ++i) {
                nodeChild1->childrens[nodeChild1->keyNum + 1 + i] = nodeChild2->childrens[i];
                nodeChild1->childrens[nodeChild1->keyNum + 1 + i]->parent = nodeChild1->childrens[nodeChild1->keyNum + 1 + i];
            }
        }
        nodeChild1->keyNum = nodeChild1->keyNum + 1 + nodeChild2->keyNum; //KEYNUM_MAX
        //父节点贡献一个
        --nodeParent->keyNum;
        //父亲结点往左边挪动
        for(int i = childFirstIndex; i < nodeParent->keyNum; ++i) {
            nodeParent->childrens[i + 1] = nodeParent->childrens[i + 2]; //当前不动, 移动下一个
            nodeParent->keyDatas[i] = nodeParent->keyDatas[i + 1];
        }
        //删除右边
        delete nodeChild2;
        nodeChild2 = nullptr;

        return nodeChild1;
    }

    void __NodeDeleteThenBalance(BTNode<T> *node, T value) {
        if(node == nullptr) return;

        int i = 0;
        for(; i < node->keyNum && value > node->keyDatas[i]; ++i) {}
        //break when value <= ... or the right
        
        if(i < node->keyNum && value == node->keyDatas[i]) {

            if(node->isLeaf) {
                //叶节点, 可以直接删除
                --node->keyNum;
                for( ;i < node->keyNum; ++i) {
                    node->keyDatas[i] = node->keyDatas[i + 1]; //keyNum已经在上一步 -1
                }
                return;
            }else{
                
                //删除结点的值, 需要补充一个值过来..
                BTNode<T> *nodeLeft = node->childrens[i]; //里面的比value小
                BTNode<T> *nodeRight = node->childrens[i + 1]; //里面的比value大

                if(nodeLeft->keyNum >= KEYNUM_MIN + 1) {
                    //BTNode<T> *tmp = __getMaxNodeInNode(nodeLeft);
                    //满足条件, 上移, 合并
                    //nodeLeft tmpMax nodeRight
                    node->keyDatas[i] = _getMaxInNode(nodeLeft);
                    //在那边去删掉那个结点, 再保持平衡. 两句的顺序是否有区别?
                    __NodeDeleteThenBalance(nodeLeft, value);
                    
                }else if(nodeRight->keyNum >= KEYNUM_MIN + 1) {
                    //右边满足条件
                    node->keyDatas[i] = _getMinInNode(nodeRight);
                    __NodeDeleteThenBalance(nodeRight, value);
                }else{
                    //都不满足, 直接合并
                    __mergeBlock(node, i);
                    //合并之后东西都在i中, 即nodeleft
                    __NodeDeleteThenBalance(nodeLeft, value);
                }
                return;

            }

        }else{ //关键值不在该node
            BTNode<T> *nodeChild = node->childrens[i]; //在这个node里找.. 跳出时, value比左大, 比右小
            BTNode<T> *nodeLeft = nullptr, *nodeRight = nullptr;

            //删除情况, 可能需要删儿子的, 但是儿子一删就太少了..
            if(nodeChild->keyNum == KEYNUM_MIN) {
                
                //给左右赋值..
                if(i > 0) nodeLeft = node->childrens[i - 1];
                if(i < KEYNUM_MIN) nodeRight = node->childrens[i + 1];

                if(nodeLeft && nodeLeft->keyNum > KEYNUM_MIN) {
                    /**
                     *         6     23
                     * 1 2 3 5  9 12   (需要删除9)
                     *       5         23
                     * 1 2 3  6 [9] 12   (此时因为要删的在p1(第二个儿子)里, 所以对其补充元素)
                    */
                    for(int j = nodeChild->keyNum; j > 0; --j) {
                        nodeChild->keyDatas[i] = nodeChild->keyDatas[i - 1];
                    }
                    //当前块, 左关键值下移
                    nodeChild->keyDatas[0] = node->keyDatas[i - 1];
                    
                    if(!nodeLeft->isLeaf) {
                        //不是叶子节点, 这一层都不会是叶子节点 //nodeChild->isLeaf == false
                        //需要处理指针块
                        for(int j = nodeChild->keyNum + 1; j > 0; --j) {
                            nodeChild->childrens[j] = nodeChild->childrens[j - 1];
                            nodeChild->childrens[j]->parent = nodeChild->childrens[j];
                        }
                        nodeChild->childrens[0] = nodeLeft->childrens[nodeLeft->keyNum];
                        nodeChild->childrens[0]->parent = nodeChild->childrens[0];
                    }

                    //处理其他
                    ++nodeChild->keyNum;
                    --nodeLeft->keyNum;
                    //处理本层
                    node->keyDatas[i - 1] = nodeLeft->keyDatas[nodeLeft->keyNum]; //"溢出"位置就是需要提上来的值..

                }else if(nodeRight && nodeRight->keyNum > KEYNUM_MIN) {
                    /**
                     *     3         23
                     * 1 2  5 6 9 12   (需要删除2)
                     *         5        23
                     * 1 [2] 3   6 9 12   (此时因为要删的在p0(第一个儿子)里, 所以对其补充元素)
                    */
                    //关键数下移
                    nodeChild->keyDatas[nodeChild->keyNum] = node->keyDatas[i];
                    //关键数上移到本层
                    node->keyDatas[i] = nodeRight->keyDatas[0];
                    //儿子(右侧), 关键数左移
                    for(int j = 0; j < nodeRight->keyNum - 1; ++j) {
                        nodeRight->keyDatas[j] = nodeRight->keyDatas[j + 1];
                    }
                    //判断是否叶子节点, 是否需要动子块
                    if(!nodeRight->isLeaf) {
                        //右子块第一个数据移动到儿子队尾
                        nodeChild->childrens[nodeChild->keyNum + 1] = nodeRight->childrens[0];
                        nodeChild->childrens[nodeChild->keyNum + 1]->parent = nodeChild->childrens[nodeChild->keyNum + 1];
                        //右子块的子块左移
                        for(int j = 0; j < nodeRight->keyNum; ++j) {
                            nodeRight->childrens[j] = nodeRight->childrens[j + 1];
                            //父亲改变
                            nodeRight->childrens[j]->parent = nodeRight->childrens[j];
                        }
                    }
                    ++nodeChild->keyNum;
                    --nodeRight->keyNum;

                }else if(nodeLeft) {
                    //左边有东西, 但是也为KEYNUM_MIN, 在临界状态
                    __mergeBlock(node, i - 1);
                    nodeChild = node->childrens[i - 1]; //需要找的东西, 已经被合并到了左边一个..
                }else if(nodeRight) {
                    //右边有东西, 但已经临界
                    //合并右兄弟
                    __mergeBlock(node, i);
                    //nodeChild = node->childrens[i]; //无需更新
                }

            }

            //已经把前置条件全部准备完毕
            __NodeDeleteThenBalance(nodeChild, value);

        }
    }


    void __printNode(BTNode<T> *node, int count) {
        if(node == nullptr) return;

        int i, j;
        for(i = 0; i < node->keyNum; ++i) {
            if(!node->isLeaf) {
                __printNode(node->childrens[i], count - 3);
            }
            for(j = count; j >= 0; --j)
                std::cout << "-";
            
            std::cout << "|" << node->keyDatas[i] << "|" << std::endl;
        }
        //多一个
        if(!node->isLeaf) {
            __printNode(node->childrens[node->keyNum], count - 3);
        }
    }


public:
    BTree(): bTreeRoot(nullptr) {}
    
    bool contain(T value) {
        return __search(bTreeRoot, value) == nullptr? false: true;
    }

    bool insert(T value) {
        return __insert(value) == nullptr? false: true;
    }

    //中序遍历
    void traversal() const{
        __traversal(bTreeRoot);
    }

    bool deleteNode(T value) {
        if(__search(bTreeRoot, value) == nullptr)
            return false;
        
        __deleteNode(value);
        return true;
    }

    void printNode() {
        __printNode(bTreeRoot, KEYNUM_MAX * 5);
    }

};

int main() {
    BTree<int> tree;
    tree.insert(1);
    tree.insert(3);

    for(int i = 10; i < 19; ++i) {
        tree.insert(i * 3);
    }

    tree.insert(57);

    tree.traversal();
    std::cout << std::endl;

    tree.printNode();

    tree.deleteNode(3);
    tree.traversal();
    std::cout << std::endl;

    return 0;
}

/**
 * 来源: https://blog.csdn.net/yantingtao_sunny/article/details/51231934
 */