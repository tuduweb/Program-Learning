#include <iostream>

constexpr int CHILDREN_MAX = 10;
constexpr int KEYNUM_MAX = CHILDREN_MAX - 1;
//最小数
constexpr int KEYNUM_MIN = KEYNUM_MAX / 2;

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
    BTNode(): keyNum(0), isLeaf(false), parent(nullptr) {
        for(int i = 0; i < CHILDREN_MAX; ++i)
            childrens[i] = nullptr;
        for(int i = 0; i < KEYNUM_MAX; ++i)
            keyDatas = 0; //这里是不是需要一个初始化的..方法?
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

        if(node->isLeaf) { //当前是叶子结点.. 那么就说明现在没儿子指针了.. 直接插入
            while(nodeKeyNum > 0 && value < node->keyDatas[nodeKeyNum - 1]) {
                //从最大的开始比较, 如果v比比较值小, 那么肯定是要插入到左边; 当前值先腾位置
                node->keyDatas[nodeKeyNum] = node->keyDatas[nodeKeyNum - 1];
                //下标移动
                --nodeKeyNum;
            }
            //此时, pos = 0, 或者找到了 value > itemData的位置..
            //因为位置已经在上一步腾出来了. 而且下标-1了, 那么就为nodeKeyNum
            node->keyDatas[nodeKeyNum] = value;
            return node;

        }else{ //不是叶子结点. 那么继续往下找, 直到找到叶子结点
            //   k0  k1 v k2      (v > data[k1], nodeKeyNum = 2 此时跳出, 且要去p2找)
            // p0  p1  p2  p3 (思考最次情况, nodeKeyNum = 0, 此时要去 p0找, keyNode为所求)
            while(nodeKeyNum > 0 && value < node->keyDatas[nodeKeyNum - 1]) {
                --nodeKeyNum;
            }
            //这时候, 找到的下标为需要往下找的儿子
            BTNode<T> *child = node->childrens[nodeKeyNum];
            if(KEYNUM_MAX == child->keyNum) {
                //想加入的地方满了, 那么需要把这个儿子块分裂, 并把儿子块中间的数提到上面来..
                //此时, 块中所有元素 小于 node->keyDatas[nodeKeyNum]
                //所以, 在分裂时, 需要从儿子里面提取一个关键数字, 到nodeKeyNum位置..
                __splitBlock(node, nodeKeyNum, child);
                
                //判断是否需要修正插入位置
                //因为把儿子给拆成了两部分, 所以需要和从儿子提上来的<关建数>比较大小, 更新位置
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
            nodeRight->keyDatas[i] = nodeChild->keyDatas[KEYNUM_MIN + 1 + i];
        }

        if(!nodeChild->isLeaf) { //不是叶子节点, 那么需要处理childrens
            //
            for(int i = 0; i < KEYNUM_MIN; ++i) {
                // nodeChild   | nodeRight
                // 0 1 2       | 3 4 5
                //             | 0 1 2 [] [] []
                nodeRight->childrens[i] = nodeChild->childrens[KEYNUM_MIN + 1 + i];
                //移动的部分, "爸爸"变了 nodeRight->childrens[0]
                nodeRight->childrens[i]->parent = nodeRight;  
            }
        }

        //关键数更新
        //nodeRight->keyNum = KEYNUM_MIN;
        nodeChild->keyNum = KEYNUM_MAX - KEYNUM_MIN; //nodeChild->keyNum - 

        //处理父节点..
        //将父节点中, 找到的插入位置之后的东西, 全部挪动 childNum = keyNum + 1
        //nodeParent
        for(int i = nodeParent->keyNum; i > childIndex; --i) {
            nodeParent->childrens[i + 1] = nodeParent->childrens[i];
            nodeParent->keyDatas[i] = nodeParent->keyDatas[i - 1];
            //nodeParent->childrens[i + 1]->parent = 
        }
        ++nodeParent->keyNum;
        //被拆分的childIndex的右边一位, 就需要放置新增的块
        //关键词是中间那一个, 把中间那一个移到上面来
        nodeParent->childrens[childIndex + 1] = nodeRight;
        nodeParent->keyDatas[childIndex] = nodeChild->keyDatas[KEY_MIN];

        return nodeRight;
    }

    bool insert(T value) {
        return __insert(bTreeRoot, value) == nullptr? false: true;
    }

    //BTNode<T> *node, 爹可能改变, 所以不能随意设置..
    BTNode<T>* __insert(T value) {
        //数据存在, 不允许插入
        //if(contain(value)) return nullptr;

        //不存在, 则新建 这里是有问题的..
        //TODO: 修改这里
        if(bTreeRoot == nullptr) bTreeRoot = new BTNode<T>();

        //爹这一层达到了最大, 提前拆成两个, 防止儿子全满了, 这种情况很难处理
        if(bTreeRoot->keyNum == KEYNUM_MAX) { //达到了最大..
            //新建一个结点
            BTNode<T> *newNode = new BTNode<T>();
            newNode->isLeaf = false; //这个结点 是要保存指针的, 所以不是叶子

            newNode->childrens[0] = node; // 容易理解, 第一个位置肯定是保存当前结点的.. data[k] > data[0] (k>0)
            //拆分当前.. 一分为二
            __splitBlock(node, KEYNUM_MIN, newNode);
            //新建的结点变成爹..
            bTreeRoot = newNode;
        }

        //重新处理插入, 这时候full的情况已经被处理, 满足notFull
        return __insert_notFull(bTreeRoot, value);

    }


public:
    BTree(): bTreeRoot(nullptr) {}
    
    bool contain(T value) {
        return __search(bTreeRoot, value) == nullptr? false: true;
    }

};

int main() {
    BTNode<int> *node;
    return 0;
}