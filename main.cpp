#include "buildPc.h"
#include "buildPc.cpp"
#include <conio.h>

using namespace std;

// ==================== ADMIN KEY ====================
const string ADMIN_KEY = "SUPERUSERDO"; // Key untuk register sebagai admin

// ==================== USER ACCOUNT ====================

struct UserAccount {
    string username;
    string password;
    int role; // 1=admin, 2=user
};

vector<UserAccount> accounts;

string getPassword() {
    string pass;
    char ch;
    while((ch = _getch()) != '\r') {
        if(ch == '\b') {
            if(!pass.empty()) {
                cout << "\b \b";
                pass.pop_back();
            }
        } else {
            pass += ch;
            cout << '*';
        }
    }
    cout << '\n';
    return pass;
}

void registerAccount() {
    cout << "\n=== REGISTER ===\n";
    string user;
    cout << "Username: ";
    cin >> user;
    flushNewline();
    
    for(auto &a : accounts) {
        if(a.username == user) {
            cout << "❌ Username sudah ada.\n";
            return;
        }
    }
    
    string pass;
    do {
        cout << "Password (min 8 char): ";
        pass = getPassword();
        if(pass.length() < 8) cout << "❌ Password kurang dari 8 karakter.\n";
    } while(pass.length() < 8);
    
    int role = getValidInput<int>("Role (1=admin, 2=user): ", 1, 2);
    
    // Jika pilih role admin, harus masukkan key
    if(role == 1) {
        cout << "Masukkan Admin Key: ";
        string key;
        cin >> key;
        flushNewline();
        
        if(key != ADMIN_KEY) {
            cout << "❌ Admin Key salah! Registrasi gagal.\n";
            return;
        }
        cout << "✅ Admin Key valid!\n";
    }
    
    accounts.push_back({user, pass, role});
    cout << "✅ Registrasi sukses sebagai " << (role == 1 ? "Admin" : "User") << ".\n";
}

UserAccount* loginAccount() {
    cout << "\n=== LOGIN ===\n";
    string user;
    cout << "Username: ";
    cin >> user;
    flushNewline();
    cout << "Password: ";
    string pass = getPassword();
    
    for(auto &a : accounts) {
        if(a.username == user && a.password == pass) return &a;
    }
    return nullptr;
}

// ==================== MENU HANDLERS ====================

void showAdminMenu() {
    cout << "\n========================================\n";
    cout << "          MENU ADMIN\n";
    cout << "========================================\n";
    cout << "1.  Tampilkan Semua Komponen\n";
    cout << "2.  Tambah Komponen\n";
    cout << "3.  Edit Komponen\n";
    cout << "4.  Hapus Komponen\n";
    cout << "5.  Cari Komponen\n";
    cout << "6.  Export Katalog (Preorder)\n";
    cout << "7.  Tampilkan Inorder\n";
    cout << "8.  Hitung Total per Kategori (Postorder)\n";
    cout << "9.  Buat Paket Build\n";
    cout << "10. Lihat Paket Build\n";
    cout << "11. Hapus Paket Build\n";
    cout << "12. Logout\n";
    cout << "========================================\n";
}

void showUserMenu() {
    cout << "\n========================================\n";
    cout << "          MENU USER\n";
    cout << "========================================\n";
    cout << "1. Tampilkan Semua Komponen\n";
    cout << "2. Cari Komponen\n";
    cout << "3. Rekomendasi Build PC\n";
    cout << "4. Logout\n";
    cout << "========================================\n";
}

void handleSearch(Node* root) {
    cout << "\n=== CARI KOMPONEN ===\n";
    string keyword = getValidStringInput("Cari (masukkan kata kunci): ");
    
    vector<pair<Node*, string>> results;
    searchNodeFuzzy(root, keyword, results);
    
    if(results.empty()) {
        cout << "!!! Tidak ditemukan komponen dengan kata kunci !!! '" << keyword << "'.\n";
        return;
    }
    
    cout << "\n Ditemukan " << results.size() << " hasil:\n";
    cout << "========================================\n";
    
    for(size_t i = 0; i < results.size(); i++) {
        Node* node = results[i].first;
        string path = results[i].second;
        
        cout << i + 1 << ". " << node->name;
        if(node->price > 0) cout << " (Rp " << node->price << ")";
        cout << "\n";
        
        if(!path.empty() && path != "PC") {
            // Remove "PC → " prefix if exists
            size_t pos = path.find("PC → ");
            if(pos == 0) path = path.substr(5);
            
            cout << "   Path: " << path << "\n";
        }
        cout << "\n";
    }
}

void handleAddComponent(Node* root) {
    cout << "\n=== TAMBAH KOMPONEN ===\n";
    string parent = getValidStringInput("Parent (kategori/merk): ");
    
    vector<string> path;
    if(!searchNode(root, parent, path)) {
        cout << "!!! Parent '" << parent << "' tidak ditemukan.\n";
        return;
    }
    
    string name = getValidStringInput("Nama komponen baru: ");
    int price = getValidInput<int>("Harga (Rp): ", 0, 999999999);
    
    if(insertNode(root, parent, name, price)) {
        cout << "✅ Komponen '" << name << "' berhasil ditambahkan.\n";
    } else {
        cout << "!!! Gagal menambahkan komponen.\n";
    }
}

void handleEditComponent(Node* root) {
    cout << "\n=== EDIT KOMPONEN ===\n";
    string oldName = getValidStringInput("Nama komponen yang ingin diedit: ");
    
    // Cek apakah komponen ada
    vector<string> path;
    Node* node = searchNode(root, oldName, path);
    
    if(!node) {
        cout << "!!! Komponen '" << oldName << "' tidak ditemukan.\n";
        return;
    }
    
    if(oldName == "PC") {
        cout << "!!! Tidak bisa mengedit root (PC)!\n";
        return;
    }
    
    // Tampilkan info komponen saat ini
    cout << "\n--- Info Komponen Saat Ini ---\n";
    cout << "Nama  : " << node->name << "\n";
    cout << "Harga : Rp " << node->price << "\n";
    cout << "Path  : ";
    for(size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if(i < path.size() - 1) cout << " → ";
    }
    cout << "\n\n";
    
    // Input data baru
    string newName = getValidStringInput("Nama baru (tekan Enter untuk tidak mengubah): ");
    if(newName.empty()) newName = oldName;
    
    cout << "Harga baru (Rp, masukkan -1 untuk tidak mengubah): ";
    int newPrice;
    cin >> newPrice;
    flushNewline();
    
    if(newPrice == -1) newPrice = node->price;
    if(newPrice < 0) newPrice = 0;
    
    // Konfirmasi
    cout << "\n--- Konfirmasi Perubahan ---\n";
    cout << "Nama  : " << oldName << " → " << newName << "\n";
    cout << "Harga : Rp " << node->price << " → Rp " << newPrice << "\n";
    cout << "Lanjutkan? (y/n): ";
    char confirm;
    cin >> confirm;
    flushNewline();
    
    if(confirm == 'y' || confirm == 'Y') {
        if(editNode(root, oldName, newName, newPrice)) {
            cout << "✅ Komponen berhasil diedit.\n";
        } else {
            cout << "!!! Gagal mengedit komponen.\n";
        }
    } else {
        cout << "❌ Edit dibatalkan.\n";
    }
}

void handleDeleteComponent(Node* root) {
    cout << "\n=== HAPUS KOMPONEN ===\n";
    cout << "⚠️  PERHATIAN: Menghapus kategori akan menghapus semua isinya!\n";
    string name = getValidStringInput("Nama yang ingin dihapus: ");
    
    if(name == "PC") {
        cout << "!!! Tidak bisa menghapus root (PC)!\n";
        return;
    }
    
    if(deleteNode(root, name)) {
        cout << "✅ '" << name << "' berhasil dihapus.\n";
    } else {
        cout << "!!! '" << name << "' tidak ditemukan.\n";
    }
}

void handleViewPackages() {
    cout << "\n";
    adminListBuildPackages();
    if(!savedBuilds.empty()) {
        int ix = getValidInput<int>("\nLihat detail paket nomor (0=kembali): ", 0, savedBuilds.size());
        if(ix > 0) {
            BuildPackage &b = savedBuilds[ix - 1];
            cout << "\n=== DETAIL PAKET ===\n";
            cout << "Nama Paket: " << b.packageName << "\n";
            cout << "Komponen:\n";
            for(auto &it : b.items) {
                cout << "  - " << it.first << " : Rp " << it.second << "\n";
            }
            cout << "----------------------------\n";
            cout << "Total: Rp " << b.totalPrice << "\n";
        }
    }
}

void adminMenuLoop(Node* root) {
    int choice = 0;
    do {
        showAdminMenu();
        choice = getValidInput<int>("Pilih: ", 1, 12);
        flushNewline();
        
        switch(choice) {
            case 1:
                cout << "\n=== TAMPILKAN SEMUA KOMPONEN ===\n";
                displayTree(root);
                break;
            case 2:
                handleAddComponent(root);
                break;
            case 3:
                handleEditComponent(root);
                break;
            case 4:
                handleDeleteComponent(root);
                break;
            case 5:
                handleSearch(root);
                break;
            case 6:
                cout << "\n=== EXPORT KATALOG (Preorder) ===\n";
                preorderPrint(root);
                cout << "Total node: " << countNodes(root) << " item\n";
                break;
            case 7:
                cout << "\n=== TAMPILKAN INORDER ===\n";
                inorder(root);
                cout << "\n";
                break;
            case 8:
                cout << "\n=== HITUNG TOTAL PER KATEGORI (Postorder) ===\n\n";
                {
                    long long grandTotal = postorder(root);
                    cout << "\n========================================\n";
                    cout << "PC | TOTAL KESELURUHAN: Rp " << grandTotal << "\n";
                    cout << "========================================\n";
                }
                break;
            case 9:
                cout << "\n=== BUAT PAKET BUILD ===\n";
                adminCreateBuildPackage(root);
                break;
            case 10:
                handleViewPackages();
                break;
            case 11:
                cout << "\n=== HAPUS PAKET BUILD ===\n";
                if(savedBuilds.empty()) {
                    cout << "!!! Belum ada paket build.\n";
                } else {
                    adminListBuildPackages();
                    string pkg = getValidStringInput("\nNama paket yang ingin dihapus: ");
                    if(adminDeleteBuildPackage(pkg)) {
                        cout << "✅ Paket '" << pkg << "' berhasil dihapus.\n";
                    } else {
                        cout << "!!! Paket '" << pkg << "' tidak ditemukan.\n";
                    }
                }
                break;
            case 12:
                cout << "\n👋 Logout berhasil.\n";
                break;
        }
    } while(choice != 12);
}

void userMenuLoop(Node* root) {
    int choice = 0;
    do {
        showUserMenu();
        choice = getValidInput<int>("Pilih: ", 1, 4);
        flushNewline();
        
        switch(choice) {
            case 1:
                cout << "\n=== TAMPILKAN SEMUA KOMPONEN ===\n";
                displayTree(root);
                break;
            case 2:
                handleSearch(root);
                break;
            case 3:
                if(savedBuilds.empty()) {
                    cout << "\n❌ Belum ada paket build dari admin.\n";
                } else {
                    handleViewPackages();
                }
                break;
            case 4:
                cout << "\n👋 Logout berhasil.\n";
                break;
        }
    } while(choice != 4);
}

// ==================== MAIN ====================

int main() {
    Node* root = loadFromCSV("components.csv");
    if(!root) {
        cout << "❌ Gagal memuat data. Pastikan file 'components.csv' ada.\n";
        return 1;
    }
    
    bool running = true;
    while(running) {
        cout << "\n========================================\n";
        cout << "     APLIKASI BUILD PC - MAIN MENU\n";
        cout << "========================================\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "========================================\n";
        
        int top = getValidInput<int>("Pilih: ", 1, 3);
        
        if(top == 1) {
            registerAccount();
        } 
        else if(top == 2) {
            UserAccount* acc = loginAccount();
            if(!acc) {
                cout << "❌ Login gagal. Username atau password salah.\n";
                continue;
            }
            cout << "\n✅ Login sukses! Selamat datang, " << acc->username << "!\n";
            
            if(acc->role == 1) {
                adminMenuLoop(root);
            } else {
                userMenuLoop(root);
            }
        } 
        else if(top == 3) {
            running = false;
        }
    }
    
    deleteSubtree(root);
    cout << "\n========================================\n";
    cout << "  Terima kasih telah menggunakan\n";
    cout << "      APLIKASI BUILD PC\n";
    cout << "========================================\n";
    return 0;
}

