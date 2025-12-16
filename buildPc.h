#ifndef BUILDPC_H
#define BUILDPC_H

#include <bits/stdc++.h>
using namespace std;

// ==================== STRUKTUR DATA ====================
struct Node {
    string name;
    int price;
    vector<Node*> children;
    Node(const string& n, int p = 0) : name(n), price(p) {}
};

struct BuildPackage {
    string packageName;
    vector<pair<string, int>> items;
    long long totalPrice;
};

extern vector<BuildPackage> savedBuilds;

// ==================== CORE FUNCTIONS ====================

// Tree Operations
void deleteSubtree(Node* root);
void displayTree(Node* root, int depth = 0);
Node* searchNode(Node* root, const string& target, vector<string>& path);
void searchNodeFuzzy(Node* root, const string& keyword, vector<pair<Node*, string>>& results, const string& parentPath = "");
bool insertNode(Node* root, const string& parentName, const string& newName, int price);
bool deleteNode(Node* root, const string& name);
int countNodes(Node* root);
int getTreeLevel(Node* root, const string& target, int level = 0);

// CSV Operations
Node* loadFromCSV(const string& filename);
void saveToCSV(Node* root, const string& filename);

// Traversal
void preorderPrint(Node* root, int depth = 0);
long long postorder(Node* root, int depth = 0);
void inorder(Node* root);

// Build PC
void collectPricedItems(Node* node, vector<Node*>& out);
void buildPcInteractive(Node* root, vector<Node*>& currentBuild);

// Admin Package
void adminCreateBuildPackage(Node* root);
void adminListBuildPackages();
bool adminDeleteBuildPackage(const string& packageName);

// Input Validation
template<typename T>
T getValidInput(const string& prompt, T minVal, T maxVal);
string getValidStringInput(const string& prompt);
void flushNewline();

#endif
