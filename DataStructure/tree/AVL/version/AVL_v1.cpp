#include <iostream>

template<typename Type>
class AVLNode{
public:
    Type data;
    AVLNode *left;
    AVLNode *right;
    AVLNode(Type _data): data(_data), left(nullptr), right(nullptr) {} 
};

template<typename Type>
class AVLTree{
    typedef AVLNode<Type> Node;
private:
    Node *avlRoot;
    int __getHeight(const Node *root); //获取树的高度
    int __diff(const Node *root); //求平衡因子

    Node* __insert(Node *&root, const Type key); //插入数据的内部实现
    Node* __deleteNode(Node *root, const Type key); //删除实现
    Node* __balance(Node* root); //平衡实现

    Node* __search(Node *node, const Type key); //查找实现

    void __deleteTree(Node *root); //删除树
    Node* __treeMax(Node *root); //前根结点最大
    Node* __treeMin(Node *root); //前根结点最小

    Node* __rotate_ll(Node* root);
    Node* __rotate_lr(Node* root);
    Node* __rotate_rr(Node* root);
    Node* __rotate_rl(Node* root);

    void __traversal(Node* root); //中序遍历

public:
    AVLTree(): avlRoot(nullptr) {}
    ~AVLTree();

    AVLTree(const Type *array, int len); //构造, 数组实现
    bool insert(const Type key); //插入接口
    bool delNode(const Type key); //删除外部接口

    void traversal();

    bool search(const Type key);

};

/**
 * 思路很简单，小于当前结点的值，往左走；
 * 大于当前结点的值，往右走。
 * 每次插入后需要进行平衡操作，保证树的平衡
 */
template<typename Type>
AVLNode<Type> *AVLTree<Type>::__insert(Node *&root, const Type key) {
    if(root == nullptr) {
        root = new Node(key);
        return root;
    }

    if(key < root->data) { // left
        __insert(root->left, key);
        root = __balance(root); //平衡当前结点..
        return root;
    }else if(key > root->data) {
        __insert(root->right, key);
        root = __balance(root);
        return root;
    }else{
        //return root;
    }

    return root;
}

template<typename Type>
bool AVLTree<Type>::insert(const Type key) {
    return __insert(avlRoot, key) == nullptr? false: true;
}

/**
 * 平衡操作
 */
//求树的高度
template<typename Type>
int AVLTree<Type>::__getHeight(const Node *root) {
    if(root == nullptr) return 0;
    return std::max(__getHeight(root->left), __getHeight(root->right)) + 1;
}

template<typename Type>
int AVLTree<Type>::__diff(const Node *root) {
    if(root == nullptr) return 0;
    return __getHeight(root->left) - __getHeight(root->right);
}

template<typename Type>
AVLNode<Type>* AVLTree<Type>::__balance(Node* root) {
    int dis = __diff(root); //平衡因子
    if(dis > 1) { //left > right
        if(__diff(root->left) > 0) {
            root = __rotate_ll(root);
        }else{
            root = __rotate_lr(root);
        }
    }else if(dis < -1) {
        if(__diff(root->right) < 0) {
            root = __rotate_rr(root);
        }else{
            root = __rotate_rl(root);
        }
    }
    return root;
}

/**
 * 旋转操作
 * 平衡二叉树的核心就是旋转操作..
 * ll,lr,rr,rl
 */
template<typename Type>
AVLNode<Type>* AVLTree<Type>::__rotate_ll(Node *root) {
    Node* tmp = root->left;
    root->left = tmp->right;
    tmp->right = root;
    return tmp;
    //out function, root = return value
}

template<typename Type>
AVLNode<Type>* AVLTree<Type>::__rotate_rr(Node *root) {
    Node *tmp = root->right;
    root->right = tmp->left; // first get tmp->left
    tmp->left = root; //override tmp->left
    return tmp;
}

template<typename Type>
AVLNode<Type>* AVLTree<Type>::__rotate_lr(Node *root) {
    //先对B结点左旋操作(rr型)
    root->left = __rotate_rr(root->left);
    //再对A结点右旋, 外部赋值
    //root = _rotate_ll(root);
    return __rotate_ll(root);
}

template<typename Type>
AVLNode<Type>* AVLTree<Type>::__rotate_rl(Node *root) {
    root->right = __rotate_ll(root->right);
    return __rotate_rr(root);
}

/**
 * 查找结点
 */
template<typename Type>
AVLNode<Type>* AVLTree<Type>::__search(Node *root, const Type key) {
    if(root == nullptr) return nullptr;
    if(key == root->data)
        return root;
    else if(key < root->data)
        return __search(root->left, key);
    else
        return __search(root->right, key);
}

//最右边那个..?
template<typename Type>
AVLNode<Type>* AVLTree<Type>::__treeMax(Node *root) {
    return root->right ? __treeMax(root->right): root;
}

template<typename Type>
AVLNode<Type>* AVLTree<Type>::__treeMin(Node *root) {
    return root->left ? __treeMin(root->left): root;
}

/**
 * 删除结点
 */
template<typename Type>
AVLNode<Type>* AVLTree<Type>::__deleteNode(Node *root, const Type key) {
    if(root == nullptr) return root;
    Node *node = __search(root, key);
    if(!node) {
        std::cout << "Key not find!" << std::endl;
        return root;
    }

    if(key == root->data) {
        if(root->left != nullptr && root->right != nullptr) {
            
            if(__diff(root) > 0) {
                //root->left里面最大的, 那么就是root的前驱结点
                root->data = __treeMax(root->left)->data;
                //上一步已经把值取出, 现在需要删除这个被抽取出来的结点, 此时那个一定会没有右子
                root->left = __deleteNode(root->left, root->data);
            }else{
                //root->right里面最小的, 对应root的后驱结点
                root->data = __treeMin(root->right)->data;
                root->right = __deleteNode(root->right, root->data);
            }

        }else{
            Node *tmp = root;
            root = root->left ? root->left : root->right;
            delete tmp;
            tmp = nullptr;
        }
    }else if(key < root->data) {
        root->left = __deleteNode(root->left, key);
        root = __balance(root);
    }else if(key > root->data) {
        root->right = __deleteNode(root->right, key);
        root = __balance(root);
    }
    return root;
}

template<typename Type>
bool AVLTree<Type>::delNode(const Type key) {
    return __deleteNode(avlRoot, key) == nullptr ? false: true;
}

template<typename Type>
void AVLTree<Type>::__deleteTree(Node *root) {
    if(root == nullptr) return;
    __deleteTree(root->left);
    __deleteTree(root->right);
    delete root;
    root = nullptr; 
}

template<typename Type>
void AVLTree<Type>::__traversal(Node* root) {
    if(root == nullptr) return;
    __traversal(root->left);
    std::cout << root->data << " ";
    __traversal(root->right);
}

template<typename Type>
void AVLTree<Type>::traversal() {
    __traversal(avlRoot);
}

/**
 * 构造, 析构
 */
template<typename Type>
AVLTree<Type>::AVLTree(const Type *array, int len): avlRoot(nullptr) {
    for(int i = 0; i < len; ++i) {
        insert(*(array + i));
    }
}
template<typename Type>
AVLTree<Type>::~AVLTree() {
    __deleteTree(avlRoot);
}

/**
 * 其他实现
 */
template<typename Type>
bool AVLTree<Type>::search(const Type key) {
    return __search(avlRoot, key) == nullptr? false: true;
}


/**
 * 测试, 实现
 */
int main() {
    int arr[] = {16,3,7,11,9,26,18,14,15};
    AVLTree<int> tree(arr, sizeof(arr)/sizeof(arr[0]));
    tree.traversal();
    std::cout << std::endl;
    tree.insert(8);
    tree.traversal();
    std::cout << std::endl;

    //search
    if(tree.search(7)) {
        std::cout << "Found!" << std::endl;
    }else{
        std::cout << "Not Found!" << std::endl;
    }

    //delete
    if(tree.delNode(7)) {
        std::cout << "Delete" << std::endl;
    }else{
        std::cout << "Not Delete" << std::endl;
    }

    if(tree.search(7)) {
        std::cout << "Found!" << std::endl;
    }else{
        std::cout << "Not Found!" << std::endl;
    }
    return 0;
}

/**
 * Source: https://www.jianshu.com/p/251008998d47
 * 平衡二叉树
 */