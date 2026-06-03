#include "tree.hpp"
#include <algorithm>
#include <functional>

// Node
Tree::Node::Node(int value, Node* p) {
    data = value;
    parent = p;
}

// Constructor
Tree::Tree(int k) {
    this->k = k;
    rootNode = nullptr;
    treeSize = 0;
}

bool Tree::isEmpty() {
    return treeSize == 0;
}

int Tree::size() {
    return treeSize;
}

int Tree::root() {
    if (!rootNode) throw std::runtime_error("Árbol vacío");
    return rootNode->data;
}

Tree::Node* Tree::search(Node* node, int value) {
    if (!node) return nullptr;
    if (node->data == value) return node;

    for (auto child : node->children) {
        Node* found = search(child, value);
        if (found) return found;
    }
    return nullptr;
}

bool Tree::insert(int parentValue, int value) {
    if (!rootNode) {
        rootNode = new Node(value);
        treeSize++;
        return true;
    }

    Node* parentNode = search(rootNode, parentValue);
    if (!parentNode) return false;

    if ((int)parentNode->children.size() >= k) return false;

    Node* newNode = new Node(value, parentNode);
    parentNode->children.push_back(newNode);
    treeSize++;
    return true;
}

int Tree::parent(int value) {
    Node* node = search(rootNode, value);
    if (!node || !node->parent)
        throw std::runtime_error("No tiene padre");

    return node->parent->data;
}

std::vector<int> Tree::children(int value) {
    Node* node = search(rootNode, value);
    std::vector<int> result;

    if (!node) return result;

    for (auto child : node->children)
        result.push_back(child->data);

    return result;
}

void Tree::deleteSubtree(Node* node) {
    if (!node) return;
    for (auto child : node->children)
        deleteSubtree(child);
    delete node;
}

bool Tree::remove(int value) {
    Node* node = search(rootNode, value);
    if (!node) return false;

    if (node == rootNode) {
        deleteSubtree(rootNode);
        rootNode = nullptr;
        treeSize = 0;
        return true;
    }

    Node* parent = node->parent;
    auto& siblings = parent->children;

    siblings.erase(
        std::remove(siblings.begin(), siblings.end(), node),
        siblings.end()
    );

    deleteSubtree(node);
    treeSize--;
    return true;
}


void Tree::preOrder(Node* node, std::vector<int>& result) {
    if (!node) return;
    result.push_back(node->data);
    for (auto child : node->children)
        preOrder(child, result);
}

std::vector<int> Tree::preOrder() {
    std::vector<int> result;
    preOrder(rootNode, result);
    return result;
}

void Tree::postOrder(Node* node, std::vector<int>& result) {
    if (!node) return;
    for (auto child : node->children)
        postOrder(child, result);
    result.push_back(node->data);
}

std::vector<int> Tree::postOrder() {
    std::vector<int> result;
    postOrder(rootNode, result);
    return result;
}

std::vector<int> Tree::inOrder() {
    std::vector<int> result;

    std::function<void(Node*)> inorder = [&](Node* node) {
        if (!node) return;

        int half = node->children.size() / 2;

        for (int i = 0; i < half; i++)
            inorder(node->children[i]);

        result.push_back(node->data);

        for (size_t i = half; i < node->children.size(); i++)
            inorder(node->children[i]);
    };

    inorder(rootNode);
    return result;
}