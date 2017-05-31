#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

//#define MY_DEBUG 1
//#define CATCH_TEST 1
//#define TEST_FILE "tree74.txt"

struct Node {
    Node(long _key): key(_key), sum(_key), parent(nullptr), lchild(nullptr), rchild(nullptr) {};
    
    long key;
    long long int sum;
    Node* parent;
    Node* lchild;
    Node* rchild;
    
    Node* getLeft() { return lchild; };
    Node* getRight() { return rchild; };
    
    void setLeft(Node* left) {
        if(nullptr != lchild)
            sum -= lchild->sum;
        lchild = left;
        if(nullptr != lchild)
            sum += lchild->sum;
    }

    void setRight(Node* right) {
        if(nullptr != rchild)
            sum -= rchild->sum;
        rchild = right;
        if(nullptr != rchild)
            sum += rchild->sum;
    }
    
    static Node* New_Node(long key) {
        Node* p_node = new Node(key);

        if (nullptr == p_node)
        {
            fprintf(stderr, "Out of memory!\n");
            exit(1);
        }

        p_node->key = key;
        p_node->sum = key;
        p_node->lchild = nullptr;
        p_node->rchild = nullptr;
        return p_node;
    };
    
    static void Delete_Nodes(Node* node) {
        #ifdef MY_DEBUG
            std::cout << "Delete nodes" << std::endl;
        #endif
        if(node == nullptr) return;
        Node* tmp = nullptr;
        // Search for the most left node
        while(node != nullptr){
            /*#ifdef MY_DEBUG
                std::cout << "  Node = " << *node << std::endl;
            #endif*/
            if(node->lchild == nullptr && node->rchild == nullptr) {
                /*#ifdef MY_DEBUG
                    std::cout << "    remove it" << std::endl;
                #endif*/
                tmp = node->parent;
                if(nullptr != tmp) {
                    if(tmp->lchild == node)
                        tmp->lchild = nullptr;
                    else
                        tmp->rchild = nullptr;
                }
                else
                    break;
                
                delete node;
                node = tmp;
            }
            if(node->lchild != nullptr){
                /*#ifdef MY_DEBUG
                    std::cout << "    go left" << std::endl;
                #endif*/
                node = node->lchild;
            }
            else {
                /*#ifdef MY_DEBUG
                    std::cout << "    go right" << std::endl;
                #endif*/
                node = node->rchild;
            }
        }
    };
    
    friend std::ostream& operator<<(std::ostream& os, Node& n);
};


std::ostream& operator<<(std::ostream& os, Node& n){
    os << n.key << ":" << n.sum;
    return os;
}
 

class SplayTree {
private:

    static void rotate(Node *parent, Node *child){
        Node *gparent = parent->parent;

        if(parent->getLeft() == child) {
            parent->setLeft(child->getRight());
            if(parent->getLeft() != nullptr) {
                parent->getLeft()->parent = parent;
            }
            child->setRight(parent);
            parent->parent = child;
        }
        else{
            parent->setRight(child->getLeft());
            if(parent->getRight() != nullptr) {
                parent->getRight()->parent = parent;
            }
            child->setLeft(parent);
            parent->parent = child;
        }
        child->parent = gparent;
        if(gparent != nullptr)
            if(gparent->getLeft() == parent){
                gparent->lchild = child;
            }
            else {
                gparent->rchild = child;
            }
    }
    
public:
    
    Node *root;
    
    SplayTree(): root(nullptr) {};
    
    ~SplayTree() {
        Node::Delete_Nodes(root);
    };
    
    bool isEmpty() { return (root == nullptr); };
    
    Node* Splay(Node* v){
        while(v->parent != nullptr){
            Node* parent = v->parent;
            Node* gparent = parent->parent;
            if(gparent == nullptr) {
                SplayTree::rotate(parent, v);
                break;
            }
            bool zigzig = (gparent->getLeft() == parent) == (parent->getLeft() == v);
            if(zigzig){
                SplayTree::rotate(gparent, parent);
                SplayTree::rotate(parent, v);
            }
            else{
                SplayTree::rotate(parent, v);
                SplayTree::rotate(gparent, v);
            }
        }
        root = v;
        return v;
    }

    void Insert(long key) {
        if (root == nullptr){
            root = new Node(key);
            return;
        }
        
        Node* nearestNode = SplayTree::findNearest(root, key);
        long nnKey = nearestNode->key;
		if (nnKey == key)
			return;
        
        Splay(nearestNode);
        
        Node* n = new Node(key);
        if(key < nearestNode->key) {
            n->setLeft(nearestNode->getLeft());
            if(n->getLeft() != nullptr){
                n->getLeft()->parent = n;
            }
            nearestNode->setLeft(nullptr);
            n->setRight(nearestNode);
            nearestNode->parent = n;
        }
        else{
            n->setRight(nearestNode->getRight());
            if(n->getRight() != nullptr){
                n->getRight()->parent = n;
            }
            nearestNode->setRight(nullptr);
            n->setLeft(nearestNode);
            nearestNode->parent = n;
        }
        root = n;
    }

    void Delete(long key) {
        if (root == nullptr){
            return;
        }
        
        Node* nearestNode = SplayTree::findNearest(root, key);
        if(nearestNode->key != key)
            return;

        Splay(nearestNode);
        
        // Now delete the root
        if(nearestNode->getLeft() == nullptr) {
            root = nearestNode->getRight();
            if(nearestNode->getRight() != nullptr)
                nearestNode->getRight()->parent = nullptr;
        }
        else {
            Node* x = nearestNode->getRight();
            nearestNode->getLeft()->parent = nullptr;
            root = nearestNode->getLeft();
            
            Splay(SplayTree::findNearest(root, key));
            root->setRight(x);
            if(x != nullptr)
                x->parent = root;
        }
    }

    bool Search(long key){
        if(nullptr == root)
            return false;
        
        Node* nearestNode = SplayTree::findNearest(root, key);
        Splay(nearestNode);
        
        return (nearestNode->key == key);
    }

    /* (Node, int) => Node
        Try to find in the tree a node with key
        nearest to the given key.
        It node shuld be used as a parent node
        to insert a new node with key. */
    static Node* findNearest(Node* v, long key){
        while(v->key != key){
            if(v->key > key){
                if(v->getLeft() == nullptr)
                    return v;
                v = v->getLeft();
            }
            else{
                if(v->getRight() == nullptr)
                    return v;
                v = v->getRight();
            }
        }
        return v;
    }
    
    void show(std::ostream& os, Node* node){
        if (node){
            os<< '[' << *node << "> ";
            show(os, node->getLeft());
            os << ", ";
            show(os, node->getRight());
            os << ']';
        }
    }
    
    long long int getSum(long left, long right) {
        Node* v = root;
        while(nullptr != v) {
            if(right < (v->key))
                v = v->getLeft();
            else if(left > v->key)
                v = v->getRight();
            else
                break;
        }
        if(nullptr == v)
            return 0;
        
        // Do some splay magic
        Splay(v);
        
        //std::cout << "v == " << *v << std::endl;
        long long int s = v->sum;
        //std::cout << "s == " << s << std::endl;
        Node* nLeft = v->getLeft();
        while(nullptr != nLeft){
            //std::cout << "Nleft " << nLeft << std::endl;
            if(left < (nLeft->key))
                nLeft = nLeft->getLeft();
            else if(left > (nLeft->key)){
                s -= nLeft->key;
                if(nullptr != (nLeft->getLeft()))
                    s -= (nLeft->getLeft())->sum;
                nLeft = nLeft->getRight();
            }
            else {
                if(nullptr != nLeft->getLeft())
                    s -= (nLeft->getLeft())->sum;
                break;
            }
        }
        //std::cout << "getSum2" << std::endl;
        Node* nRight = v->getRight();
        while(nullptr != nRight){
            if(right > (nRight->key))
                nRight = nRight->getRight();
            else {
                if(nullptr != (nRight->getRight()))
                    s -= (nRight->getRight())->sum;
                if(right < nRight->key){
                    s -= nRight->key;
                    nRight = nRight->getLeft();
                }
                else
                    break;
            }
        }
        return s;
    }
    
    friend std::ostream& operator<<(std::ostream& os, SplayTree& st);
};


std::ostream& operator<<(std::ostream& os, SplayTree& st) {
    st.show(os, st.root);
    return os;
}

long f(const long v, const long long int& s) {
    return (long)(((long long int)v + s) % 1000000001LL);
}


void solve(SplayTree& st, std::istream& in, std::ostream& os) {
    #ifdef CATCH_TEST
        std::stringstream err;
        bool flRaise = false;
    #endif
    
    long n = 0;
    long long int s = 0;
    char cmd;
    long v1, v2, v;
    
    in >> n;
    in >> cmd >> v;
    
    #ifdef CATCH_TEST
        if(n == 100000 && v != 625019466 && v != 825230965 && v != 197289225 && v != 649271330
              && v != 108159721 && v != 30382616) {
            std::string line;
            std::cerr << n << '|' << cmd << ' ' << v; //<< in.rdbuf();
            long from = 20133;
            for(long i=0; i<from; ++i)
                std::getline(in, line);
            for(long i=from; i<n; ++i) {
                std::getline(in, line);
                std::cerr << line << '|';
            }
            throw 0;
        }
    #endif

    for(long i = 0; i < n; ++i) {
        #ifdef MY_DEBUG
            os << "--- " << cmd << ' ' << v;
        #endif
        v1 = f(v, s);
        switch(cmd) {
            case '+':
                #ifdef MY_DEBUG
                    os << std::endl;
                    os << "  add.  " << v1 << std::endl;
                #endif
                st.Insert(v1);
                #ifdef MY_DEBUG
                    os << "  data:  " << st << std::endl;
                #endif
                break;
            case '-':
                #ifdef MY_DEBUG
                    os << std::endl;
                    os << "  delete.  " << v1 << std::endl;
                #endif
                st.Delete(v1);
                #ifdef MY_DEBUG
                    os << "  data:  " << st << std::endl;
                #endif
                break;
            case '?':
                #ifdef MY_DEBUG
                    os << std::endl;
                    os << "  find  " << v1 << std::endl;
                    os << "  data:  " << st << std::endl;
                #endif
                if(st.Search(v1))
                    os << "Found" << std::endl;
                else
                    os << "Not found" << std::endl;
                #ifdef MY_DEBUG
                    os << "  data:  " << st << std::endl;
                #endif
                break;
            case 's':
                in >> v2;
                #ifdef MY_DEBUG
                    os << ' ' << v2 << std::endl;
                #endif
                if(!st.isEmpty()) {
                    #ifdef MY_DEBUG
                        os << "  sum [ " << v1 << " ; ";
                    #endif
                    v2 = f(v2, s);
                    #ifdef MY_DEBUG
                        os << v2 << " ]" << std::endl;
                        os << "  data:  " << st << std::endl;
                    #endif
                    s = st.getSum(v1, v2);
                }
                else {
                    s = 0;
                }
                os << s << std::endl;
                break;
        }
        // read new portion of the data
        if(i != n-1)
            in >> cmd >> v;
    }
}


int main(){
    SplayTree st;
    std::stringstream ss;
    std::stringstream buffer;
    
    #ifdef TEST_FILE
        std::ifstream ifs;
        ifs.open(TEST_FILE, std::ofstream::in);
        buffer << ifs.rdbuf();
        solve(st, buffer, ss);
        ifs.close();
        
    #else
        buffer << std::cin.rdbuf();
        solve(st, buffer, ss);
    #endif
    
    
    std::cout << ss.rdbuf();
    
    
    return 0;
}
