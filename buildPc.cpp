#include "buildPc.h"

vector<BuildPackage> savedBuilds;

// ==================== INPUT VALIDATION ====================

template<typename T>
T getValidInput(const string& prompt, T minVal, T maxVal) {
    T val;
    while(true) {
        cout << prompt;
        if(cin >> val && val >= minVal && val <= maxVal) return val;
        cout << "❌ Input harus antara " << minVal << " dan " << maxVal << ".\n";
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

template int getValidInput<int>(const string&, int, int);

string getValidStringInput(const string& prompt) {
    string input;
    while(true) {
        cout << prompt;
        getline(cin, input);
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);
        if(!input.empty()) return input;
        cout << "❌ Input tidak boleh kosong!\n";
    }
}

void flushNewline() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ==================== TREE UTILITIES ====================

void deleteSubtree(Node* root) {
    if(!root) return;
    for(Node* c : root->children) deleteSubtree(c);
    delete root;
}

void displayTree(Node* root, int depth) {
    if(!root) return;
    cout << string(depth * 4, ' ') << "- " << root->name;
    if(root->price > 0) cout << " (Rp " << root->price << ")";
    cout << "\n";
    for(Node* c : root->children) displayTree(c, depth + 1);
}

int countNodes(Node* root) {
    if(!root) return 0;
    int c = 1;
    for(Node* child : root->children) c += countNodes(child);
    return c;
}

int getTreeLevel(Node* root, const string& target, int level) {
    if(!root) return -1;
    if(root->name == target) return level;
    for(Node* c : root->children) {
        int result = getTreeLevel(c, target, level + 1);
        if(result != -1) return result;
    }
    return -1;
}

// ==================== CSV OPERATIONS ====================

Node* loadFromCSV(const string& filename) {
    ifstream file(filename);
    if(!file.is_open()) {
        cout << "❌ Gagal membuka file: " << filename << "\n";
        return nullptr;
    }
    
    Node* root = new Node("PC");
    map<string, Node*> nodeMap;
    nodeMap["PC"] = root;
    
    string line;
    getline(file, line); // Skip header
    
    while(getline(file, line)) {
        if(line.empty()) continue;
        stringstream ss(line);
        string level, parent, name, priceStr;
        
        getline(ss, level, ',');
        getline(ss, parent, ',');
        getline(ss, name, ',');
        getline(ss, priceStr, ',');
        
        int price = priceStr.empty() ? 0 : stoi(priceStr);
        Node* newNode = new Node(name, price);
        
        if(nodeMap.find(parent) != nodeMap.end()) {
            nodeMap[parent]->children.push_back(newNode);
            nodeMap[name] = newNode;
        }
    }
    
    file.close();
    cout << "✅ Data berhasil dimuat dari " << filename << "\n";
    return root;
}

void saveNodeToCSV(Node* node, ofstream& f, int lvl, const string& parent) {
    if(lvl > 0) f << lvl << "," << parent << "," << node->name << "," << node->price << "\n";
    for(Node* c : node->children) saveNodeToCSV(c, f, lvl + 1, node->name);
}

void saveToCSV(Node* root, const string& filename) {
    ofstream file(filename);
    if(!file.is_open()) {
        cout << "❌ Gagal menyimpan ke file: " << filename << "\n";
        return;
    }
    file << "level,parent,name,price\n";
    saveNodeToCSV(root, file, 0, "");
    file.close();
    cout << "✅ Data berhasil disimpan ke " << filename << "\n";
}

// ==================== TRAVERSAL ====================

void preorderPrint(Node* root, int depth) {
    if(!root) return;
    cout << string(depth * 2, ' ') << root->name;
    if(root->price > 0) cout << " (Rp " << root->price << ")";
    cout << "\n";
    for(Node* c : root->children) preorderPrint(c, depth + 1);
}

long long postorder(Node* root, int depth) {
    if(!root) return 0;
    long long total = 0;
    for(Node* c : root->children) total += postorder(c, depth + 1);
    total += root->price;
    
    cout << root->name;
    if(root->price > 0) cout << " (Rp " << root->price << ")";
    cout << "\n";
    
    if(!root->children.empty() && total > 0) {
        cout << root->name << " | TOTAL: Rp " << total << "\n";
        if(depth == 1) cout << "---\n";
    }
    return total;
}

void inorder(Node* root) {
    if(!root) return;
    if(!root->children.empty()) inorder(root->children[0]);
    cout << root->name;
    if(root->price > 0) cout << " (Rp " << root->price << ")";
    cout << " | ";
    for(size_t i = 1; i < root->children.size(); i++) inorder(root->children[i]);
}

// ==================== SEARCH & MODIFY ====================

Node* searchNode(Node* root, const string& target, vector<string>& path) {
    if(!root) return nullptr;
    path.push_back(root->name);
    if(root->name == target) return root;
    
    for(Node* c : root->children) {
        vector<string> subpath;
        Node* found = searchNode(c, target, subpath);
        if(found) {
            for(size_t i = 1; i < subpath.size(); i++) path.push_back(subpath[i]);
            return found;
        }
    }
    path.pop_back();
    return nullptr;
}

void searchNodeFuzzy(Node* root, const string& keyword, vector<pair<Node*, string>>& results, const string& parentPath) {
    if(!root) return;
    
    // Convert to lowercase for case-insensitive search
    string lowerName = root->name;
    string lowerKeyword = keyword;
    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
    
    // Check if keyword is found in node name
    if(lowerName.find(lowerKeyword) != string::npos) {
        results.push_back({root, parentPath});
    }
    
    // Build new parent path for children
    string newPath = parentPath.empty() ? root->name : parentPath + " → " + root->name;
    
    // Search in children
    for(Node* c : root->children) {
        searchNodeFuzzy(c, keyword, results, newPath);
    }
}

bool insertNode(Node* root, const string& parentName, const string& newName, int price) {
    vector<string> p;
    Node* parent = searchNode(root, parentName, p);
    if(!parent) return false;
    parent->children.push_back(new Node(newName, price));
    return true;
}

bool findParent(Node* root, const string& target, Node*& parentOut, int& idxOut) {
    if(!root) return false;
    for(size_t i = 0; i < root->children.size(); i++) {
        if(root->children[i]->name == target) {
            parentOut = root;
            idxOut = i;
            return true;
        }
        if(findParent(root->children[i], target, parentOut, idxOut)) return true;
    }
    return false;
}

bool deleteNode(Node* root, const string& name) {
    if(!root) return false;
    Node* parent = nullptr;
    int idx = -1;
    
    if(findParent(root, name, parent, idx)) {
        Node* del = parent->children[idx];
        deleteSubtree(del);
        parent->children.erase(parent->children.begin() + idx);
        return true;
    }
    return false;
}

// ==================== BUILD PC ====================

void collectPricedItems(Node* node, vector<Node*>& out) {
    if(!node) return;
    if(node->price > 0) {
        out.push_back(node);
        return;
    }
    for(Node* c : node->children) collectPricedItems(c, out);
}

void buildPcInteractive(Node* root, vector<Node*>& currentBuild) {
    currentBuild.clear();
    cout << "=== BUILD PC (Interactive) ===\n";
    
    for(Node* category : root->children) {
        cout << "\nKategori: " << category->name << "\n";
        vector<Node*> items;
        collectPricedItems(category, items);
        
        if(items.empty()) {
            cout << "  (Tidak ada item)\n";
            currentBuild.push_back(nullptr);
            continue;
        }
        
        for(int i = 0; i < (int)items.size(); i++) {
            cout << " " << i + 1 << ". " << items[i]->name << " (Rp " << items[i]->price << ")\n";
        }
        
        int pilih = getValidInput<int>("Pilih (0 untuk skip): ", 0, items.size());
        currentBuild.push_back(pilih == 0 ? nullptr : items[pilih - 1]);
    }
    
    cout << "\n--- Ringkasan Build ---\n";
    long long total = 0;
    for(int i = 0; i < (int)root->children.size(); i++) {
        cout << root->children[i]->name << ": ";
        if(!currentBuild[i]) cout << "(skip)\n";
        else {
            cout << currentBuild[i]->name << " (Rp " << currentBuild[i]->price << ")\n";
            total += currentBuild[i]->price;
        }
    }
    cout << "Total Harga: Rp " << total << "\n";
}

// ==================== ADMIN PACKAGE ====================

void adminCreateBuildPackage(Node* root) {
    if(!root) {
        cout << "Tree kosong.\n";
        return;
    }
    
    BuildPackage pkg;
    cout << "Masukkan nama Build PC: ";
    pkg.packageName = getValidStringInput("");
    pkg.totalPrice = 0;
    
    for(Node* category : root->children) {
        cout << "\nKategori: " << category->name << "\n";
        vector<Node*> items;
        collectPricedItems(category, items);
        
        if(items.empty()) {
            cout << "  (Tidak ada item di kategori ini)\n";
            continue;
        }
        
        for(int i = 0; i < (int)items.size(); i++) {
            cout << " " << i + 1 << ". " << items[i]->name << " (Rp " << items[i]->price << ")\n";
        }
        
        int pilih = getValidInput<int>("Pilih item (0 = skip): ", 0, items.size());
        if(pilih == 0) continue;
        
        Node* chosen = items[pilih - 1];
        pkg.items.push_back({chosen->name, chosen->price});
        pkg.totalPrice += chosen->price;
    }
    
    savedBuilds.push_back(pkg);
    cout << "✅ Paket '" << pkg.packageName << "' berhasil disimpan. Total: Rp " << pkg.totalPrice << "\n";
}

void adminListBuildPackages() {
    if(savedBuilds.empty()) {
        cout << "Belum ada paket build yang disimpan.\n";
        return;
    }
    cout << "=== Daftar Paket Build ===\n";
    for(size_t i = 0; i < savedBuilds.size(); i++) {
        cout << i + 1 << ". " << savedBuilds[i].packageName 
             << " (Rp " << savedBuilds[i].totalPrice << ")\n";
    }
}

bool adminDeleteBuildPackage(const string& packageName) {
    for(auto it = savedBuilds.begin(); it != savedBuilds.end(); ++it) {
        if(it->packageName == packageName) {
            savedBuilds.erase(it);
            return true;
        }
    }
    return false;
}