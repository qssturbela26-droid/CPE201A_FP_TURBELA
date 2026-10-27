#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

using namespace std;

// ---------------------------------------------------------------------------
// Data structures
// ---------------------------------------------------------------------------

// A product that can be bought and stored
struct Product {
    int id;
    string name;
    string category;
    string brand;
    string size;
    double price;
};

// A storage room / warehouse where products are kept
struct Storage {
    int id;
    string name;
    string location;
};

// A record of one purchase of a product
struct Purchase {
    int id;
    int productId;
    int quantity;
    string brand;
    string size;
    string date;
    string supplier;
    string purchaseLocation;
};

// Links a product to a storage and tracks how many are stored there
struct Inventory {
    int productId;
    int storageId;
    int quantity;
};

// ---------------------------------------------------------------------------
// Input helpers
// ---------------------------------------------------------------------------

// Shows a prompt and keeps asking until the user types a valid whole number.
// Prevents the program from getting stuck when letters are typed by mistake.
int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(10000, '\n');   // discard the rest of the line
            return value;
        }
        if (cin.eof()) exit(0);        // input closed, stop the program
        cout << "Invalid input. Please enter a number.\n";
        cin.clear();                   // reset the error state
        cin.ignore(10000, '\n');       // throw away the bad input
    }
}

// Same as readInt(), but for decimal numbers (e.g. prices)
double readDouble(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(10000, '\n');
            return value;
        }
        if (cin.eof()) exit(0);
        cout << "Invalid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// ---------------------------------------------------------------------------
// Main class
// ---------------------------------------------------------------------------
class InventorySystem {
private:
    vector<Product> products;
    vector<Storage> storages;
    vector<Purchase> purchases;
    vector<Inventory> inventory;

    // File names used for saving/loading data
    const string PRODUCTS_FILE = "products.txt";
    const string STORAGES_FILE = "storages.txt";
    const string PURCHASES_FILE = "purchases.txt";
    const string INVENTORY_FILE = "inventory.txt";

    // ----------------------------- Saving ----------------------------------

    // Writes all products to products.txt (one product per line, comma-separated)
    void saveProducts() {
        ofstream outFile(PRODUCTS_FILE);
        for (const auto& p : products) {
            outFile << p.id << "," << p.name << "," << p.brand << ","
                    << p.size << "," << p.category << "," << p.price << "\n";
        }
    }

    // Writes all storages to storages.txt
    void saveStorages() {
        ofstream outFile(STORAGES_FILE);
        for (const auto& s : storages) {
            outFile << s.id << "," << s.name << "," << s.location << "\n";
        }
    }

    // Writes all purchase records to purchases.txt
    void savePurchases() {
        ofstream outFile(PURCHASES_FILE);
        for (const auto& p : purchases) {
            outFile << p.id << "," << p.productId << "," << p.quantity << ","
                    << p.brand << "," << p.size << "," << p.date << ","
                    << p.supplier << "," << p.purchaseLocation << "\n";
        }
    }

    // Writes all inventory records to inventory.txt
    void saveInventoryFile() {
        ofstream outFile(INVENTORY_FILE);
        for (const auto& i : inventory) {
            outFile << i.productId << "," << i.storageId << "," << i.quantity << "\n";
        }
    }

    // ----------------------------- Loading ---------------------------------

    // Reads products.txt into the products vector (does nothing if the file is missing)
    void loadProducts() {
        ifstream inFile(PRODUCTS_FILE);
        if (!inFile.is_open()) return;
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string idStr, name, brand, size, cat, priceStr;
            if (getline(ss, idStr, ',') && getline(ss, name, ',') &&
                getline(ss, brand, ',') && getline(ss, size, ',') &&
                getline(ss, cat, ',') && getline(ss, priceStr, ',')) {
                products.push_back({stoi(idStr), name, cat, brand, size, stod(priceStr)});
            }
        }
    }

    // Reads storages.txt into the storages vector.
    // If the file doesn't exist yet, creates three default storages and saves them.
    void loadStorages() {
        ifstream inFile(STORAGES_FILE);
        if (!inFile.is_open()) {
            storages.push_back({1, "Appliances Storage", "Warehouse A"});
            storages.push_back({2, "Household Storage", "Warehouse B"});
            storages.push_back({3, "Food Storage", "Warehouse C"});
            saveStorages();
            return;
        }
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string idStr, name, loc;
            if (getline(ss, idStr, ',') && getline(ss, name, ',') && getline(ss, loc, ',')) {
                storages.push_back({stoi(idStr), name, loc});
            }
        }
    }

    // Reads purchases.txt into the purchases vector
    void loadPurchases() {
        ifstream inFile(PURCHASES_FILE);
        if (!inFile.is_open()) return;
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string idStr, prodIdStr, qtyStr, brand, size, date, supplier, loc;
            if (getline(ss, idStr, ',') && getline(ss, prodIdStr, ',') && getline(ss, qtyStr, ',') &&
                getline(ss, brand, ',') && getline(ss, size, ',') && getline(ss, date, ',') &&
                getline(ss, supplier, ',') && getline(ss, loc, ',')) {
                purchases.push_back({stoi(idStr), stoi(prodIdStr), stoi(qtyStr), brand, size, date, supplier, loc});
            }
        }
    }

    // Reads inventory.txt into the inventory vector
    void loadInventoryFile() {
        ifstream inFile(INVENTORY_FILE);
        if (!inFile.is_open()) return;
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string prodIdStr, storIdStr, qtyStr;
            if (getline(ss, prodIdStr, ',') && getline(ss, storIdStr, ',') && getline(ss, qtyStr, ',')) {
                inventory.push_back({stoi(prodIdStr), stoi(storIdStr), stoi(qtyStr)});
            }
        }
    }

    // ------------------------- Helper functions ----------------------------

    // Returns true if a product with the given ID exists
    bool productExists(int id) {
        for (const auto& p : products) {
            if (p.id == id) return true;
        }
        return false;
    }

    // Returns true if a storage with the given ID exists
    bool storageExists(int id) {
        for (const auto& s : storages) {
            if (s.id == id) return true;
        }
        return false;
    }

    // Returns true if a purchase with the given ID exists
    bool purchaseExists(int id) {
        for (const auto& p : purchases) {
            if (p.id == id) return true;
        }
        return false;
    }

    // Adds quantity to an existing product/storage inventory record,
    // or creates a new record if none exists yet. Saves the file afterwards.
    void addToInventory(int productId, int storageId, int qty) {
        for (auto &item : inventory) {
            if (item.productId == productId && item.storageId == storageId) {
                item.quantity += qty;
                saveInventoryFile();
                return;
            }
        }
        inventory.push_back({productId, storageId, qty});
        saveInventoryFile();
    }

    // Asks a yes/no question; returns true only if the user answers 'y' or 'Y'
    bool confirm(const string& message) {
        char c;
        cout << message << " (y/n): ";
        cin >> c;
        cin.ignore(10000, '\n');
        return (c == 'y' || c == 'Y');
    }
	// SUMMER PART
    // ------------------------- Delete functions ----------------------------
    // These are private: they are only reached through deleteMenu() below.

    // Deletes a product by ID. Also removes every inventory and purchase
    // record linked to that product, after asking the user to confirm.
    void deleteProduct() {
        int id = readInt("\nProduct ID to delete: ");

        if (!productExists(id)) {
            cout << "Error: Product ID " << id << " does not exist.\n";
            return;
        }

        // Count the linked records so the user knows what else will be deleted
        int invCount = count_if(inventory.begin(), inventory.end(),
                                [id](const Inventory& i) { return i.productId == id; });
        int purCount = count_if(purchases.begin(), purchases.end(),
                                [id](const Purchase& p) { return p.productId == id; });

        cout << "This will also delete " << invCount << " inventory record(s) and "
             << purCount << " purchase record(s) linked to this product.\n";

        if (!confirm("Are you sure you want to delete this product?")) {
            cout << "Deletion cancelled.\n";
            return;
        }

        // erase-remove idiom: removes all matching items from each vector
        products.erase(remove_if(products.begin(), products.end(),
                       [id](const Product& p) { return p.id == id; }), products.end());
        inventory.erase(remove_if(inventory.begin(), inventory.end(),
                        [id](const Inventory& i) { return i.productId == id; }), inventory.end());
        purchases.erase(remove_if(purchases.begin(), purchases.end(),
                        [id](const Purchase& p) { return p.productId == id; }), purchases.end());

        saveProducts();
        saveInventoryFile();
        savePurchases();

        cout << "Product deleted successfully.\n";
    }

    // Deletes a storage by ID. Also removes the inventory records stored in it,
    // after asking the user to confirm.
    void deleteStorage() {
        int id = readInt("\nStorage ID to delete: ");

        if (!storageExists(id)) {
            cout << "Error: Storage ID " << id << " does not exist.\n";
            return;
        }

        int invCount = count_if(inventory.begin(), inventory.end(),
                                [id](const Inventory& i) { return i.storageId == id; });

        cout << "This will also delete " << invCount << " inventory record(s) stored here.\n";

        if (!confirm("Are you sure you want to delete this storage?")) {
            cout << "Deletion cancelled.\n";
            return;
        }

        storages.erase(remove_if(storages.begin(), storages.end(),
                       [id](const Storage& s) { return s.id == id; }), storages.end());
        inventory.erase(remove_if(inventory.begin(), inventory.end(),
                        [id](const Inventory& i) { return i.storageId == id; }), inventory.end());

        saveStorages();
        saveInventoryFile();

        cout << "Storage deleted successfully.\n";
    }

    // Deletes a single purchase record by ID (after confirmation).
    // Note: this does not change the inventory quantities.
    void deletePurchase() {
        int id = readInt("\nPurchase ID to delete: ");

        if (!purchaseExists(id)) {
            cout << "Error: Purchase ID " << id << " does not exist.\n";
            return;
        }

        if (!confirm("Are you sure you want to delete this purchase record?")) {
            cout << "Deletion cancelled.\n";
            return;
        }

        purchases.erase(remove_if(purchases.begin(), purchases.end(),
                        [id](const Purchase& p) { return p.id == id; }), purchases.end());

        savePurchases();

        cout << "Purchase deleted successfully.\n";
    }

    // Deletes one inventory record, identified by product ID + storage ID
    // (after showing its current quantity and asking for confirmation).
    void deleteInventory() {
        int prodId = readInt("\nProduct ID: ");
        int storId = readInt("Storage ID: ");

        // Find the matching product/storage record
        auto it = find_if(inventory.begin(), inventory.end(),
                          [=](const Inventory& i) { return i.productId == prodId && i.storageId == storId; });

        if (it == inventory.end()) {
            cout << "Error: No inventory record found for Product " << prodId
                 << " in Storage " << storId << ".\n";
            return;
        }

        cout << "Current quantity: " << it->quantity << endl;

        if (!confirm("Are you sure you want to delete this inventory record?")) {
            cout << "Deletion cancelled.\n";
            return;
        }

        inventory.erase(it);
        saveInventoryFile();

        cout << "Inventory record deleted successfully.\n";
    }

public:
    // Constructor: loads all data from the text files when the program starts
    InventorySystem() {
        loadStorages();
        loadProducts();
        loadPurchases();
        loadInventoryFile();
    }

    // ------------------------------ Add ------------------------------------
	// BRIONES PART
    // Asks for product details and adds a new product (ID must be unique)
    void addProduct() {
        Product p;
        p.id = readInt("\nProduct ID: ");

        if (productExists(p.id)) {
            cout << "Error: A product with ID " << p.id << " already exists!\n";
            return;
        }

        cout << "Product Name: ";
        getline(cin, p.name);

        cout << "Brand: ";
        getline(cin, p.brand);

        cout << "Size: ";
        getline(cin, p.size);

        cout << "Category: ";
        getline(cin, p.category);

        p.price = readDouble("Price: ");

        products.push_back(p);
        saveProducts();

        cout << "Product added successfully.\n";
    }
	// BRIONES PART
    // Asks for storage details and adds a new storage (ID must be unique)
    void addStorage() {
        Storage s;
        s.id = readInt("\nStorage ID: ");

        if (storageExists(s.id)) {
            cout << "Error: A storage with ID " << s.id << " already exists!\n";
            return;
        }

        cout << "Storage Name: ";
        getline(cin, s.name);

        cout << "Location: ";
        getline(cin, s.location);

        storages.push_back(s);
        saveStorages();

        cout << "Storage added successfully.\n";
    }
 	//CORTEZ PART
    // Records a purchase of an existing product, copies the brand and size from
    // the product, and automatically adds the quantity to the chosen storage
    void recordPurchase() {
        Purchase p;
        p.id = readInt("\nPurchase ID: ");

        p.productId = readInt("Product ID: ");

        if (!productExists(p.productId)) {
            cout << "Error: Product ID " << p.productId << " does not exist. Create the product first!\n";
            return;
        }

        // Fill in brand and size automatically from the product
        for (const auto& prod : products) {
            if (prod.id == p.productId) {
                p.brand = prod.brand;
                p.size = prod.size;
                break;
            }
        }

        p.quantity = readInt("Quantity: ");

        int storageId = readInt("Storage ID (where the items will be stored): ");

        if (!storageExists(storageId)) {
            cout << "Error: Storage ID " << storageId << " does not exist. Create the storage room first!\n";
            return;
        }

        cout << "Purchase Date (YYYY-MM-DD): ";
        getline(cin, p.date);

        cout << "Supplier: ";
        getline(cin, p.supplier);

        cout << "Purchase Location: ";
        getline(cin, p.purchaseLocation);

        purchases.push_back(p);
        savePurchases();

        // Update stock in the chosen storage
        addToInventory(p.productId, storageId, p.quantity);

        cout << "Purchase recorded and inventory updated successfully.\n";
    }

    // ----------------------------- Delete menu -----------------------------
	// SUMMER PART
    // Shows a submenu listing what can be deleted and runs the chosen delete function
    void deleteMenu() {
        cout << "\n========== DELETE MENU ==========\n";
        cout << "1. Delete Product\n";
        cout << "2. Delete Storage\n";
        cout << "3. Delete Purchase\n";
        cout << "4. Delete Inventory Record\n";
        cout << "0. Back\n";
        int choice = readInt("Choose: ");

        switch (choice) {
            case 1: deleteProduct(); break;
            case 2: deleteStorage(); break;
            case 3: deletePurchase(); break;
            case 4: deleteInventory(); break;
            case 0: cout << "Returning to main menu.\n"; break;
            default: cout << "Invalid choice.\n";
        }
    }

    // ----------------------------- Display ---------------------------------
    
    // CHIARA PART

    // Prints all inventory records as a table (product ID, storage ID, quantity)
    void displayInventory() {
        cout << "\n========== INVENTORY ==========\n";
        cout << left << setw(12) << "Product ID" << setw(12) << "Storage ID" << setw(12) << "Quantity" << endl;
        cout << "----------------------------------------\n";
        for (const auto &item : inventory) {
            cout << left << setw(12) << item.productId << setw(12) << item.storageId << setw(12) << item.quantity << endl;
        }
    }
	// CORTEZ
    // Prints the details of every product
    void displayProducts() {
        cout << "\n========== PRODUCTS ==========\n";
        for (const auto &p : products) {
            cout << "ID: " << p.id << endl;
            cout << "Name: " << p.name << endl;
            cout << "Brand: " << p.brand << endl;
            cout << "Size: " << p.size << endl;
            cout << "Category: " << p.category << endl;
            cout << "Price: php" << fixed << setprecision(2) << p.price << endl;
            cout << "-----------------------------\n";
        }
    }
	// REDGE PART
    // Prints the details of every storage
    void displayStorages() {
        cout << "\n========== STORAGES ==========\n";
        for (const auto &s : storages) {
            cout << "ID: " << s.id << endl;
            cout << "Name: " << s.name << endl;
            cout << "Location: " << s.location << endl;
            cout << "-----------------------------\n";
        }
    }
	// AXLE PART
    // Prints the full purchase history
    void displayPurchases() {
        cout << "\n========== PURCHASE HISTORY ==========\n";
        for (const auto &p : purchases) {
            cout << "Purchase ID: " << p.id << endl;
            cout << "Product ID: " << p.productId << endl;
            cout << "Quantity: " << p.quantity << endl;
            cout << "Brand: " << p.brand << endl;
            cout << "Size: " << p.size << endl;
            cout << "Date: " << p.date << endl;
            cout << "Supplier: " << p.supplier << endl;
            cout << "Location: " << p.purchaseLocation << endl;
            cout << "-----------------------------\n";
        }
    }
};

// ---------------------------------------------------------------------------
// Program entry point: shows the main menu in a loop until the user picks 0
// ---------------------------------------------------------------------------
int main() {
    InventorySystem system;
    int choice;

    do {
        cout << "\n========== INVENTORY MANAGEMENT SYSTEM ==========\n";
        cout << "1. Add Product\n";
        cout << "2. Add Storage\n";
        cout << "3. Record Purchase\n";
        cout << "4. Display Products\n";
        cout << "5. Display Inventory\n";
        cout << "6. Display Purchase History\n";
        cout << "7. Display Storages\n";
        cout << "8. Delete\n";
        cout << "0. Exit\n";
        choice = readInt("Choose: ");

        switch (choice) {
            case 1: system.addProduct(); break;
            case 2: system.addStorage(); break;
            case 3: system.recordPurchase(); break;
            case 4: system.displayProducts(); break;
            case 5: system.displayInventory(); break;
            case 6: system.displayPurchases(); break;
            case 7: system.displayStorages(); break;
            case 8: system.deleteMenu(); break;
            case 0: cout << "Program closed.\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}
