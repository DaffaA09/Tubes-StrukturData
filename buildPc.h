#ifndef BUILDPC_H
#define BUILDPC_H

#include <bits/stdc++.h>
using namespace std;

struct Node {
    string name;
    int price;
    vector<Node*> children;

    Node(const string& n, int p = 0) : name(n), price(p) {}
};


// Tree utility
void deleteSubtree(Node* root);
void displayTree(Node* root, int depth = 0);

// Traversal
void preorder(Node* root);
void inorder(Node* root);
void postorder(Node* root);

// Searching
Node* searchNode(Node* root, const string& target, vector<string>& path);
bool findParent(Node* root, const string& target, Node*& parentOut, int& idxOut);

// Insert / delete
bool insertNode(Node* root, const string& parentName, const string& newName, int price);
bool deleteNode(Node* root, const string& name);

// Stats
int countNodes(Node* root);
int height(Node* root);

// Build initial tree
Node* buildInitialTree();

// Build PC tools
void collectPricedItems(Node* node, vector<Node*>& out);
void buildPcInteractive(Node* root, vector<Node*>& currentBuild);
void resetBuild(vector<Node*>& currentBuild);

// Utility
void flushNewline();

#endif
