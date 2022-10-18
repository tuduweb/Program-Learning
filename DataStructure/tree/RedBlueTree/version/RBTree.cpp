#include <iostream>

enum class RBTNodeColor{
    red = 0u,
    black = 1u
};

// template<class T> //简化, 去掉template实现
class RBTNode{
public:
    RBTNodeColor color;
    int key; //关键字
    RBTNode *left;
    RBTNode *right;
    RBTNode *parent;
    RBTNode(): color(RBTNodeColor::red), left(nullptr), right(nullptr) {}
    RBTNode(RBTNodeColor _color): color(_color), left(nullptr), right(nullptr) {}

    //static RBTNode *NIL;
};
//static RBTNode *NIL = new RBTNode(RBTNodeColor::black);

class RBTree{
private:
    RBTNode *root;
    RBTNode *NIL;

    RBTNode* AllocNode(int value) {
        RBTNode *node = new RBTNode;
        if(node == nullptr) return nullptr;
        node->left = node->right = node->parent = NIL;
        node->key = value;
        return node;
    }
public:
    RBTree(): NIL(new RBTNode(RBTNodeColor::black)) { root = NIL; } //如果用初始化列表实现?

    /* 左转，对z结点左转
     *       zp                 zp
     *       /                  /
     *     z                   y
     *    / \      ===>       / \
     *   lz  y               z   ry
     *      / \             / \
     *     ly  ry          lz  ly  
    */
    void LeftRotate(RBTNode *z) {
        RBTNode *y = z->right;
        z->right = y->left;

        //操作parent前需要先判断
        if(y->left != NIL) {
            y->left->parent = z;
        }
        y->parent = z->parent;

        if(root == z) {
            //z无parent
            root = y;
        }else if(z == z->parent->left) {
            z->parent->left = y;
        }else{
            z->parent->right = y;
        }

        y->left = z;
        z->parent = y;
    }

    /**
     * 右转，对z结点进行右转
     *         zp               zp
     *        /                 /
     *       z                 y
     *      / \    ===>       / \
     *     y   rz           ly   z   
     *    / \                   / \
     *   ly  ry                ry  rz
    */
    void RightRotate(RBTNode *z) {
        RBTNode *y = z->left;
        z->left = y->right;
        if(y->right != NIL) {
            y->right->parent = z;
        }
        y->parent = z->parent;
        if(root == z) {
            root = y;
        }else if(z == z->parent->left) {
            z->parent->left = y;
        }else{
            z->parent->right = y;
        }
        y->right = z;
        z->parent = y;
    }

    bool Insert(int value) {
        RBTNode *parentNode = NIL;
        RBTNode *cur = root;
        while(cur != NIL) {
            if(value == cur->key)
                return false;
            
            parentNode = cur;

            if(value < cur->key)
                cur = cur->left;
            else
                cur = cur->right;
        }
        //达成退出条件, cur == NIL
        cur = AllocNode(value);
        if(parentNode == NIL) {
            root = cur;
            root->parent = parentNode;
        }else{
            //父结点不是根节点..
            if(value < parentNode->key) {
                parentNode->left = cur;
            }else{
                parentNode->right = cur;
            }
        }

        //调整平衡, 12种情况
        FixBalanceWhenInsert(cur);
        return true;
    }

    /**
     * 6.2.8 插入情况总结
     * 插入一共有12种情况：
     * 插入节点的父节点是黑色的情况有4种
     * 这种情况仍然会维持红黑树的性质，则不需要进行额外处理。
     * 插入节点的父节点是红色的情况有8种
     * 这种情况不满足红黑树的性质4，需要进行额外的修复处理。
     * 这8种情况中：
     * 叔父节点不是红色的情况有4种
     * 这些情况都是非上溢，需要通过重新染色和旋转来进行修复
     * 叔父节点是红色的情况有4种
     * 这些情况都是上溢的，只需要通过祖父节点上溢合并和染色即可完成修复
     * 
     * @param node 待插入的节点
    */
    void FixBalanceWhenInsert(RBTNode *node) {
        RBTNode *uncleNode; //父亲的兄弟节点..
        //能到这里, 是一定有父节点的
        //插入结点的父结点是红色节点的有八种情况。当parent是黑色不需要进行额外处理. 直接跳出
        while(node->parent->color == RBTNodeColor::red) {
            //兄弟节点的判断
            if(node->parent == node->parent->parent->left) {
                uncleNode = node->parent->parent->right;

                //判定条件：uncle 是红色节点。满足这个条件的就都是上溢的情况，上溢的修复只需要染色，不需要旋转。
                if(uncleNode->color == RBTNodeColor::red) {
                    //为红色 都是上溢的，只需要通过祖父节点上溢合并和染色即可完成修复
                    //parent、uncle 染成黑色; grand 向上合并:染成<红色/.>，当做是新添加的节点进行处理
                    node->parent->color = RBTNodeColor::black;
                    uncleNode->color = RBTNodeColor::black;
                    node->parent->parent->color = RBTNodeColor::red;
                    //结点指向祖父结点.. 当做是新添加的结点处理 ->while
                    node = node->parent->parent;
                }else{
                    //为Nil或者结点颜色为黑色 LL情况 LR情况
                    if(node == node->parent->right) {
                        //LR情况：父节点为祖父节点的左节点，插入节点为父节点的右节点
                        //LR：grand 右旋转, parent 左旋转
                        node = node->parent;
                        LeftRotate(node);
                        //旋转过后, node还是原来的node, 上上句的影响消除..
                    }
                    //LL情况: grand 右单旋
                    node->parent->color = RBTNodeColor::black;
                    node->parent->parent->color = RBTNodeColor::red;
                    RightRotate(node->parent->parent);
                }

            }else if(node->parent == node->parent->parent->right) {
                //R开头的情况(RL/RR/上溢RL/上溢RR)
                node->parent == node->parent->parent->left;

                if(uncleNode->color == RBTNodeColor::red) {
                    node->parent->color = RBTNodeColor::black;
                    uncleNode->color = RBTNodeColor::black;
                    
                    node->parent->parent->color = RBTNodeColor::red;
                    node = node->parent->parent;
                }else{
                    //上溢
                    if(node == node->parent->left) {
                        //RL
                        node = node->parent;
                        RightRotate(node);
                    }
                    node->parent->color = RBTNodeColor::black;
                    node->parent->parent->color = RBTNodeColor::red;
                    LeftRotate(node->parent->parent);
                }
            }

        }

        root->color = RBTNodeColor::black; //根节点始终为黑色

    }

    void InOrder(RBTNode *cur) {
        if(cur == NIL)
            return;

        InOrder(cur->left);
        std::cout << cur->key << " ";
        InOrder(cur->right);
    }

    void InOrder() {
        std::cout << std::endl;
        InOrder(root);
        std::cout << std::endl;
    }

};

//class

int main(int argc, char** argv) {
    RBTree tree;

    for(int i = 0; i < 10; ++i) {
        tree.Insert(100 - i * 3);
    }
    tree.Insert(1);

    tree.InOrder();
    return 0;
}