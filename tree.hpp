#include <vector>
#include <iostream>

class Tree {
private:
    struct Node {
        int data;
        Node* parent;
        std::vector<Node*> children;

        Node(int value, Node* p = nullptr);
    };

    Node* rootNode;
    int treeSize;
    int k;

    void preOrder(Node* node, std::vector<int>& result);
    void postOrder(Node* node, std::vector<int>& result);
    void deleteSubtree(Node* node); //si se borra un nodo, se borran todos sus hijos y luego ese nodo.

public:
    Tree(int k);

    bool isEmpty();
    int size();

    int root();

    int parent(int value);
    std::vector<int> children(int value);

    bool insert(int parentValue, int value);
    bool remove(int value);
    
    Node* search(Node* node, int value);

    std::vector<int> preOrder();
    std::vector<int> postOrder();
    std::vector<int> inOrder();
};