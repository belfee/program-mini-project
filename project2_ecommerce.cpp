// ================================================================
// Project 2 - Smart E-Commerce System
// All-in-one single file (combined .h + .cpp)
// ================================================================

#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <ctime>

// -------- Product.h --------

class Product {
private:
    int         mId;
    std::string mName;
    std::string mCategory;
    double      mPrice;
    int         mStock;
    int         mViewCount;
    int         mPurchaseCount;

public:
    Product();
    Product(int id, const std::string& name, const std::string& category,
            double price, int stock);

    // Getters
    int         GetId()            const { return mId; }
    std::string GetName()          const { return mName; }
    std::string GetCategory()      const { return mCategory; }
    double      GetPrice()         const { return mPrice; }
    int         GetStock()         const { return mStock; }
    int         GetViewCount()     const { return mViewCount; }
    int         GetPurchaseCount() const { return mPurchaseCount; }

    // Setters
    void SetName(const std::string& name)         { mName = name; }
    void SetCategory(const std::string& category) { mCategory = category; }
    void SetPrice(double price)                    { mPrice = price; }
    void SetStock(int stock)                       { mStock = stock; }

    void IncrementView()     { mViewCount++; }
    void IncrementPurchase(int qty = 1);

    // Recommendation score: score = view*0.2 + purchase*0.8 + category_bonus
    double GetScore(double categoryBonus = 0.0) const;

    // Serialization
    std::string Serialize()   const;
    static Product Deserialize(const std::string& line);

    void Print() const;
};


// -------- Product.cpp --------

Product::Product()
    : mId(0), mName(""), mCategory(""), mPrice(0.0),
      mStock(0), mViewCount(0), mPurchaseCount(0) {}

Product::Product(int id, const std::string& name, const std::string& category,
                 double price, int stock)
    : mId(id), mName(name), mCategory(category), mPrice(price),
      mStock(stock), mViewCount(0), mPurchaseCount(0) {}

void Product::IncrementPurchase(int qty) {
    mPurchaseCount += qty;
    mStock         -= qty;
    if (mStock < 0) mStock = 0;
}

double Product::GetScore(double categoryBonus) const {
    return mViewCount * 0.2 + mPurchaseCount * 0.8 + categoryBonus;
}

// Format: id|name|category|price|stock|viewCount|purchaseCount
std::string Product::Serialize() const {
    std::ostringstream oss;
    oss << mId << "|" << mName << "|" << mCategory << "|"
        << std::fixed << std::setprecision(2) << mPrice << "|"
        << mStock << "|" << mViewCount << "|" << mPurchaseCount;
    return oss.str();
}

Product Product::Deserialize(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '|')) tokens.push_back(token);
    if (tokens.size() < 7) throw std::runtime_error("Invalid product line: " + line);

    Product p;
    p.mId            = std::stoi(tokens[0]);
    p.mName          = tokens[1];
    p.mCategory      = tokens[2];
    p.mPrice         = std::stod(tokens[3]);
    p.mStock         = std::stoi(tokens[4]);
    p.mViewCount     = std::stoi(tokens[5]);
    p.mPurchaseCount = std::stoi(tokens[6]);
    return p;
}

void Product::Print() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "[" << mId << "] " << mName
              << " | Category: " << mCategory
              << " | Price: $" << mPrice
              << " | Stock: " << mStock
              << " | Views: " << mViewCount
              << " | Purchases: " << mPurchaseCount << "\n";
}


// -------- User.h --------

class User {
private:
    int         mId;
    std::string mUsername;
    std::string mPassword;   // stored as plain text for demo purposes
    bool        mIsAdmin;

    // interaction history: productId -> {viewCount, purchaseCount}
    std::map<int, std::pair<int,int>> mHistory;

public:
    User();
    User(int id, const std::string& username,
         const std::string& password, bool isAdmin = false);

    // Getters
    int         GetId()       const { return mId; }
    std::string GetUsername() const { return mUsername; }
    std::string GetPassword() const { return mPassword; }
    bool        IsAdmin()     const { return mIsAdmin; }

    // Interaction tracking
    void RecordView(int productId);
    void RecordPurchase(int productId, int qty = 1);
    int  GetViewCount(int productId)     const;
    int  GetPurchaseCount(int productId) const;

    // Returns all product IDs the user interacted with
    std::vector<int> GetInteractedProducts() const;

    // Serialization
    std::string Serialize()   const;
    static User Deserialize(const std::string& line);

    void Print() const;
};


// -------- User.cpp --------

User::User() : mId(0), mUsername(""), mPassword(""), mIsAdmin(false) {}

User::User(int id, const std::string& username,
           const std::string& password, bool isAdmin)
    : mId(id), mUsername(username), mPassword(password), mIsAdmin(isAdmin) {}

void User::RecordView(int productId) {
    mHistory[productId].first++;
}

void User::RecordPurchase(int productId, int qty) {
    mHistory[productId].second += qty;
}

int User::GetViewCount(int productId) const {
    auto it = mHistory.find(productId);
    return (it != mHistory.end()) ? it->second.first : 0;
}

int User::GetPurchaseCount(int productId) const {
    auto it = mHistory.find(productId);
    return (it != mHistory.end()) ? it->second.second : 0;
}

std::vector<int> User::GetInteractedProducts() const {
    std::vector<int> ids;
    for (auto& kv : mHistory) ids.push_back(kv.first);
    return ids;
}

// Format: id|username|password|isAdmin|pid:v:p,pid:v:p,...
std::string User::Serialize() const {
    std::ostringstream oss;
    oss << mId << "|" << mUsername << "|" << mPassword << "|"
        << (mIsAdmin ? 1 : 0) << "|";
    bool first = true;
    for (auto& kv : mHistory) {
        if (!first) oss << ",";
        oss << kv.first << ":" << kv.second.first << ":" << kv.second.second;
        first = false;
    }
    return oss.str();
}

User User::Deserialize(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '|')) tokens.push_back(token);
    if (tokens.size() < 5) throw std::runtime_error("Invalid user line: " + line);

    User u;
    u.mId       = std::stoi(tokens[0]);
    u.mUsername = tokens[1];
    u.mPassword = tokens[2];
    u.mIsAdmin  = (tokens[3] == "1");

    // Parse history: pid:v:p,pid:v:p
    if (!tokens[4].empty()) {
        std::istringstream hist(tokens[4]);
        std::string entry;
        while (std::getline(hist, entry, ',')) {
            std::istringstream es(entry);
            std::string part;
            std::vector<std::string> parts;
            while (std::getline(es, part, ':')) parts.push_back(part);
            if (parts.size() == 3)
                u.mHistory[std::stoi(parts[0])] = { std::stoi(parts[1]), std::stoi(parts[2]) };
        }
    }
    return u;
}

void User::Print() const {
    std::cout << "[" << mId << "] " << mUsername
              << (mIsAdmin ? " (Admin)" : " (Customer)")
              << " | Interactions: " << mHistory.size() << "\n";
}


// -------- ShoppingCart.h --------

// --- CartItem -----------------------------------------------------------------

struct CartItem {
    int    productId;
    std::string productName;
    double price;
    int    quantity;

    CartItem(int id, const std::string& name, double price, int qty)
        : productId(id), productName(name), price(price), quantity(qty) {}

    double Subtotal() const { return price * quantity; }
    void Print() const;
};

// --- ShoppingCart -------------------------------------------------------------

class ShoppingCart {
private:
    int               mUserId;
    std::vector<CartItem> mItems;

public:
    explicit ShoppingCart(int userId);

    // Operations
    void AddItem(int productId, const std::string& name, double price, int qty = 1);
    void RemoveItem(int productId);
    void UpdateQuantity(int productId, int newQty);
    void Clear();

    // Queries
    bool   IsEmpty()     const { return mItems.empty(); }
    int    GetUserId()   const { return mUserId; }
    double GetTotal()    const;
    int    GetItemCount() const { return (int)mItems.size(); }
    const std::vector<CartItem>& GetItems() const { return mItems; }

    void Print() const;
};


// -------- ShoppingCart.cpp --------

// --- CartItem ----------------------------------------------------------------

void CartItem::Print() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  [" << productId << "] " << productName
              << " x" << quantity
              << "  @$" << price
              << "  = $" << Subtotal() << "\n";
}

// --- ShoppingCart -------------------------------------------------------------

ShoppingCart::ShoppingCart(int userId) : mUserId(userId) {}

void ShoppingCart::AddItem(int productId, const std::string& name,
                           double price, int qty) {
    for (auto& item : mItems) {
        if (item.productId == productId) {
            item.quantity += qty;
            return;
        }
    }
    mItems.emplace_back(productId, name, price, qty);
}

void ShoppingCart::RemoveItem(int productId) {
    mItems.erase(
        std::remove_if(mItems.begin(), mItems.end(),
            [productId](const CartItem& i){ return i.productId == productId; }),
        mItems.end());
}

void ShoppingCart::UpdateQuantity(int productId, int newQty) {
    for (auto& item : mItems) {
        if (item.productId == productId) {
            if (newQty <= 0)
                RemoveItem(productId);
            else
                item.quantity = newQty;
            return;
        }
    }
}

void ShoppingCart::Clear() {
    mItems.clear();
}

double ShoppingCart::GetTotal() const {
    double total = 0.0;
    for (auto& item : mItems) total += item.Subtotal();
    return total;
}

void ShoppingCart::Print() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "+--- Shopping Cart (User " << mUserId << ") ---------------+\n";
    if (mItems.empty()) {
        std::cout << "  (empty)\n";
    } else {
        for (auto& item : mItems) item.Print();
        std::cout << "  -----------------------------------------\n";
        std::cout << "  TOTAL: $" << GetTotal() << "\n";
    }
    std::cout << "+---------------------------------------------+\n";
}


// -------- Order.h --------

class Order {
private:
    int                   mOrderId;
    int                   mUserId;
    std::vector<CartItem> mItems;
    double                mTotal;
    std::string           mTimestamp;

    static int sNextId;

    static std::string CurrentTimestamp();

public:
    Order(int userId, const std::vector<CartItem>& items, double total);

    // Getters
    int         GetOrderId()   const { return mOrderId; }
    int         GetUserId()    const { return mUserId; }
    double      GetTotal()     const { return mTotal; }
    std::string GetTimestamp() const { return mTimestamp; }
    const std::vector<CartItem>& GetItems() const { return mItems; }

    // Serialization
    std::string Serialize()   const;
    static Order Deserialize(const std::string& line);

    static void SetNextId(int id) { sNextId = id; }
    static int  GetNextId()       { return sNextId; }

    void Print() const;
};


// -------- Order.cpp --------

int Order::sNextId = 1;

std::string Order::CurrentTimestamp() {
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buf);
}

Order::Order(int userId, const std::vector<CartItem>& items, double total)
    : mOrderId(sNextId++), mUserId(userId),
      mItems(items), mTotal(total),
      mTimestamp(CurrentTimestamp()) {}

// Format: orderId|userId|total|timestamp|pid:name:price:qty,...
std::string Order::Serialize() const {
    std::ostringstream oss;
    oss << mOrderId << "|" << mUserId << "|"
        << std::fixed << std::setprecision(2) << mTotal << "|"
        << mTimestamp << "|";
    for (size_t i = 0; i < mItems.size(); ++i) {
        if (i > 0) oss << ",";
        oss << mItems[i].productId << ":"
            << mItems[i].productName << ":"
            << mItems[i].price << ":"
            << mItems[i].quantity;
    }
    return oss.str();
}

Order Order::Deserialize(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '|')) tokens.push_back(token);
    if (tokens.size() < 5) throw std::runtime_error("Invalid order line");

    std::vector<CartItem> items;
    if (!tokens[4].empty()) {
        std::istringstream itemStream(tokens[4]);
        std::string entry;
        while (std::getline(itemStream, entry, ',')) {
            std::istringstream es(entry);
            std::string part;
            std::vector<std::string> parts;
            while (std::getline(es, part, ':')) parts.push_back(part);
            if (parts.size() == 4)
                items.emplace_back(std::stoi(parts[0]), parts[1],
                                   std::stod(parts[2]), std::stoi(parts[3]));
        }
    }

    double total = std::stod(tokens[2]);
    Order o(std::stoi(tokens[1]), items, total);
    o.mOrderId   = std::stoi(tokens[0]);
    o.mTimestamp = tokens[3];
    return o;
}

void Order::Print() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Order #" << mOrderId
              << " | User: " << mUserId
              << " | " << mTimestamp
              << " | Total: $" << mTotal << "\n";
    for (auto& item : mItems) item.Print();
}


// -------- RecommendationEngine.h --------

// --- RecommendationEngine -----------------------------------------------------
// Input : user interaction history (views + purchases)
// Output: Top N recommended products
// Score : view_count*0.2 + purchase_count*0.8 + category_bonus

class RecommendationEngine {
private:
    // Category bonus: based on how much the user likes this category
    double CategoryBonus(const User& user,
                         const std::string& category,
                         const std::vector<Product>& allProducts) const;

public:
    // Returns top N products sorted by score (excludes already-purchased items)
    std::vector<Product> Recommend(const User&                user,
                                   const std::vector<Product>& allProducts,
                                   int topN = 5) const;

    void PrintRecommendations(const User&                user,
                              const std::vector<Product>& allProducts,
                              int topN = 5) const;
};


// -------- RecommendationEngine.cpp --------

// --- Category bonus -----------------------------------------------------------
// Bonus = (total purchases in same category by user) * 0.5

double RecommendationEngine::CategoryBonus(
        const User& user,
        const std::string& category,
        const std::vector<Product>& allProducts) const {

    double bonus = 0.0;
    for (auto& p : allProducts) {
        if (p.GetCategory() == category) {
            int purchased = user.GetPurchaseCount(p.GetId());
            int viewed    = user.GetViewCount(p.GetId());
            bonus += purchased * 0.5 + viewed * 0.1;
        }
    }
    return bonus;
}

// --- Recommend ----------------------------------------------------------------

std::vector<Product> RecommendationEngine::Recommend(
        const User& user,
        const std::vector<Product>& allProducts,
        int topN) const {

    // Compute score for each product
    std::vector<std::pair<double, int>> scored;   // (score, index)
    for (size_t i = 0; i < allProducts.size(); ++i) {
        const Product& p = allProducts[i];
        if (p.GetStock() <= 0) continue;          // skip out-of-stock

        double catBonus = CategoryBonus(user, p.GetCategory(), allProducts);
        double userViews    = user.GetViewCount(p.GetId());
        double userPurchases = user.GetPurchaseCount(p.GetId());

        // Score based on global stats + user's own history in this category
        double score = p.GetViewCount()     * 0.2
                     + p.GetPurchaseCount() * 0.8
                     + catBonus
                     + userViews    * 0.3   // boost if user already viewed
                     + userPurchases * 0.1; // small boost for re-purchase

        scored.push_back({ score, (int)i });
    }

    // Sort descending by score
    std::sort(scored.begin(), scored.end(),
              [](const std::pair<double,int>& a, const std::pair<double,int>& b){
                  return a.first > b.first;
              });

    std::vector<Product> result;
    int count = 0;
    for (auto& s : scored) {
        if (count >= topN) break;
        result.push_back(allProducts[s.second]);
        count++;
    }
    return result;
}

void RecommendationEngine::PrintRecommendations(
        const User& user,
        const std::vector<Product>& allProducts,
        int topN) const {

    std::cout << "\n>> Top " << topN << " recommendations for " << user.GetUsername() << ":\n";
    auto recs = Recommend(user, allProducts, topN);
    if (recs.empty()) {
        std::cout << "  No recommendations available.\n";
        return;
    }
    int rank = 1;
    for (auto& p : recs) {
        std::cout << "  " << rank++ << ". ";
        p.Print();
    }
}


// -------- ECommerceSystem.h --------

// --- ECommerceSystem ----------------------------------------------------------
// Central system managing products, users, carts, orders, and recommendations.

class ECommerceSystem {
private:
    std::vector<Product>          mProducts;
    std::vector<User>             mUsers;
    std::vector<Order>            mOrders;
    std::map<int, ShoppingCart>   mCarts;    // userId -> cart
    RecommendationEngine          mEngine;

    // File paths for persistence
    std::string mProductFile;
    std::string mUserFile;
    std::string mOrderFile;

    // ID counters
    int mNextProductId;
    int mNextUserId;

    // Helpers
    Product* FindProduct(int id);
    User*    FindUser(int id);
    User*    FindUserByName(const std::string& username);
    void     EnsureCart(int userId);

public:
    ECommerceSystem(const std::string& productFile = "data/products.txt",
                    const std::string& userFile    = "data/users.txt",
                    const std::string& orderFile   = "data/orders.txt");

    // -- Persistence -----------------------------------------------------------
    void LoadAll();
    void SaveAll() const;

    // -- Auth ------------------------------------------------------------------
    User* Login(const std::string& username, const std::string& password);
    bool  RegisterUser(const std::string& username, const std::string& password);

    // -- Customer operations ---------------------------------------------------
    void BrowseProducts(int userId);
    void ViewProduct(int userId, int productId);
    void AddToCart(int userId, int productId, int qty = 1);
    void RemoveFromCart(int userId, int productId);
    void UpdateCartQty(int userId, int productId, int newQty);
    void ViewCart(int userId);
    Order* Checkout(int userId);
    void ViewOrderHistory(int userId) const;
    void GetRecommendations(int userId, int topN = 5);

    // -- Admin operations ------------------------------------------------------
    void AddProduct(const std::string& name, const std::string& category,
                    double price, int stock);
    void EditProduct(int productId, const std::string& name,
                     const std::string& category, double price, int stock);
    void DeleteProduct(int productId);
    void ViewStatsMostViewed(int topN = 5)    const;
    void ViewStatsBestSelling(int topN = 5)   const;
    void ViewStatsActiveUsers(int topN = 5)   const;
    void ListAllProducts()                    const;
    void ListAllUsers()                       const;
};


// -------- ECommerceSystem.cpp --------

// --- Constructor ------------------------------------------------------------------

ECommerceSystem::ECommerceSystem(const std::string& productFile,
                                 const std::string& userFile,
                                 const std::string& orderFile)
    : mProductFile(productFile), mUserFile(userFile),
      mOrderFile(orderFile), mNextProductId(1), mNextUserId(1) {}

// --- Helpers ----------------------------------------------------------------------

Product* ECommerceSystem::FindProduct(int id) {
    for (auto& p : mProducts)
        if (p.GetId() == id) return &p;
    return nullptr;
}

User* ECommerceSystem::FindUser(int id) {
    for (auto& u : mUsers)
        if (u.GetId() == id) return &u;
    return nullptr;
}

User* ECommerceSystem::FindUserByName(const std::string& username) {
    for (auto& u : mUsers)
        if (u.GetUsername() == username) return &u;
    return nullptr;
}

void ECommerceSystem::EnsureCart(int userId) {
    if (mCarts.find(userId) == mCarts.end())
        mCarts.emplace(userId, ShoppingCart(userId));
}

// --- Persistence ------------------------------------------------------------------

void ECommerceSystem::LoadAll() {
    // Products
    {
        std::ifstream f(mProductFile);
        if (f.is_open()) {
            std::string line;
            int maxId = 0;
            while (std::getline(f, line)) {
                if (line.empty()) continue;
                try {
                    Product p = Product::Deserialize(line);
                    maxId = std::max(maxId, p.GetId());
                    mProducts.push_back(p);
                } catch (...) {}
            }
            mNextProductId = maxId + 1;
        }
    }
    // Users
    {
        std::ifstream f(mUserFile);
        if (f.is_open()) {
            std::string line;
            int maxId = 0;
            while (std::getline(f, line)) {
                if (line.empty()) continue;
                try {
                    User u = User::Deserialize(line);
                    maxId = std::max(maxId, u.GetId());
                    mUsers.push_back(u);
                } catch (...) {}
            }
            mNextUserId = maxId + 1;
        }
    }
    // Orders
    {
        std::ifstream f(mOrderFile);
        if (f.is_open()) {
            std::string line;
            int maxId = 0;
            while (std::getline(f, line)) {
                if (line.empty()) continue;
                try {
                    Order o = Order::Deserialize(line);
                    maxId = std::max(maxId, o.GetOrderId());
                    mOrders.push_back(o);
                } catch (...) {}
            }
            Order::SetNextId(maxId + 1);
        }
    }
}

void ECommerceSystem::SaveAll() const {
    auto writeFile = [](const std::string& path,
                        const std::vector<std::string>& lines) {
        std::ofstream f(path);
        for (auto& l : lines) f << l << "\n";
    };

    std::vector<std::string> pLines, uLines, oLines;
    for (auto& p : mProducts) pLines.push_back(p.Serialize());
    for (auto& u : mUsers)    uLines.push_back(u.Serialize());
    for (auto& o : mOrders)   oLines.push_back(o.Serialize());

    writeFile(mProductFile, pLines);
    writeFile(mUserFile,    uLines);
    writeFile(mOrderFile,   oLines);
}

// --- Auth -------------------------------------------------------------------------

User* ECommerceSystem::Login(const std::string& username,
                             const std::string& password) {
    User* u = FindUserByName(username);
    if (u && u->GetPassword() == password) return u;
    return nullptr;
}

bool ECommerceSystem::RegisterUser(const std::string& username,
                                   const std::string& password) {
    if (FindUserByName(username)) return false;   // username taken
    mUsers.emplace_back(mNextUserId++, username, password, false);
    return true;
}

// --- Customer operations ----------------------------------------------------------

void ECommerceSystem::BrowseProducts(int /*userId*/) {
    std::cout << "\n--- Product Catalog -----------------------------------\n";
    if (mProducts.empty()) { std::cout << "  No products available.\n"; return; }
    for (auto& p : mProducts) {
        if (p.GetStock() > 0) p.Print();
    }
    std::cout << "-------------------------------------------------------\n";
}

void ECommerceSystem::ViewProduct(int userId, int productId) {
    Product* p = FindProduct(productId);
    if (!p) { std::cout << "Product not found.\n"; return; }

    p->IncrementView();
    User* u = FindUser(userId);
    if (u) u->RecordView(productId);

    std::cout << "\n-- Product Details --\n";
    p->Print();
}

void ECommerceSystem::AddToCart(int userId, int productId, int qty) {
    Product* p = FindProduct(productId);
    if (!p) { std::cout << "Product not found.\n"; return; }
    if (p->GetStock() < qty) {
        std::cout << "Not enough stock (available: " << p->GetStock() << ")\n";
        return;
    }
    EnsureCart(userId);
    mCarts.at(userId).AddItem(productId, p->GetName(), p->GetPrice(), qty);
    std::cout << "Added " << qty << "x [" << p->GetName() << "] to cart.\n";
}

void ECommerceSystem::RemoveFromCart(int userId, int productId) {
    EnsureCart(userId);
    mCarts.at(userId).RemoveItem(productId);
    std::cout << "Item removed from cart.\n";
}

void ECommerceSystem::UpdateCartQty(int userId, int productId, int newQty) {
    EnsureCart(userId);
    mCarts.at(userId).UpdateQuantity(productId, newQty);
    std::cout << "Cart updated.\n";
}

void ECommerceSystem::ViewCart(int userId) {
    EnsureCart(userId);
    mCarts.at(userId).Print();
}

Order* ECommerceSystem::Checkout(int userId) {
    EnsureCart(userId);
    ShoppingCart& cart = mCarts.at(userId);
    if (cart.IsEmpty()) {
        std::cout << "Cart is empty!\n";
        return nullptr;
    }

    // Deduct stock and record purchases
    for (auto& item : cart.GetItems()) {
        Product* p = FindProduct(item.productId);
        if (p) p->IncrementPurchase(item.quantity);
        User*    u = FindUser(userId);
        if (u) u->RecordPurchase(item.productId, item.quantity);
    }

    mOrders.emplace_back(userId, cart.GetItems(), cart.GetTotal());
    Order* created = &mOrders.back();

    cart.Clear();
    std::cout << "\n>> Order placed successfully!\n";
    created->Print();
    return created;
}

void ECommerceSystem::ViewOrderHistory(int userId) const {
    std::cout << "\n--- Order History (User " << userId << ") ---------------\n";
    bool found = false;
    for (auto& o : mOrders) {
        if (o.GetUserId() == userId) { o.Print(); found = true; }
    }
    if (!found) std::cout << "  No orders yet.\n";
}

void ECommerceSystem::GetRecommendations(int userId, int topN) {
    User* u = FindUser(userId);
    if (!u) { std::cout << "User not found.\n"; return; }
    mEngine.PrintRecommendations(*u, mProducts, topN);
}

// --- Admin operations -------------------------------------------------------------

void ECommerceSystem::AddProduct(const std::string& name,
                                  const std::string& category,
                                  double price, int stock) {
    mProducts.emplace_back(mNextProductId++, name, category, price, stock);
    std::cout << "Product [" << name << "] added (ID=" << mNextProductId-1 << ").\n";
}

void ECommerceSystem::EditProduct(int productId, const std::string& name,
                                   const std::string& category,
                                   double price, int stock) {
    Product* p = FindProduct(productId);
    if (!p) { std::cout << "Product not found.\n"; return; }
    p->SetName(name);
    p->SetCategory(category);
    p->SetPrice(price);
    p->SetStock(stock);
    std::cout << "Product [" << productId << "] updated.\n";
}

void ECommerceSystem::DeleteProduct(int productId) {
    auto it = std::remove_if(mProducts.begin(), mProducts.end(),
        [productId](const Product& p){ return p.GetId() == productId; });
    if (it == mProducts.end()) { std::cout << "Product not found.\n"; return; }
    mProducts.erase(it, mProducts.end());
    std::cout << "Product [" << productId << "] deleted.\n";
}

void ECommerceSystem::ViewStatsMostViewed(int topN) const {
    std::cout << "\n--- Most Viewed Products (Top " << topN << ") -----------\n";
    auto sorted = mProducts;
    std::sort(sorted.begin(), sorted.end(),
        [](const Product& a, const Product& b){ return a.GetViewCount() > b.GetViewCount(); });
    for (int i = 0; i < topN && i < (int)sorted.size(); ++i) {
        std::cout << i+1 << ". "; sorted[i].Print();
    }
}

void ECommerceSystem::ViewStatsBestSelling(int topN) const {
    std::cout << "\n--- Best Selling Products (Top " << topN << ") ----------\n";
    auto sorted = mProducts;
    std::sort(sorted.begin(), sorted.end(),
        [](const Product& a, const Product& b){ return a.GetPurchaseCount() > b.GetPurchaseCount(); });
    for (int i = 0; i < topN && i < (int)sorted.size(); ++i) {
        std::cout << i+1 << ". "; sorted[i].Print();
    }
}

void ECommerceSystem::ViewStatsActiveUsers(int topN) const {
    std::cout << "\n--- Most Active Users (Top " << topN << ") --------------\n";
    auto sorted = mUsers;
    std::sort(sorted.begin(), sorted.end(),
        [](const User& a, const User& b){
            return a.GetInteractedProducts().size() > b.GetInteractedProducts().size();
        });
    for (int i = 0; i < topN && i < (int)sorted.size(); ++i) {
        std::cout << i+1 << ". "; sorted[i].Print();
    }
}

void ECommerceSystem::ListAllProducts() const {
    std::cout << "\n--- All Products ---------------------------------------\n";
    for (auto& p : mProducts) p.Print();
}

void ECommerceSystem::ListAllUsers() const {
    std::cout << "\n--- All Users -----------------------------------------\n";
    for (auto& u : mUsers) u.Print();
}


// -------- main.cpp --------

static void sep(const std::string& title) {
    std::cout << "\n==================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "==================================================\n";
}

int main() {
    ECommerceSystem sys("data/products.txt",
                        "data/users.txt",
                        "data/orders.txt");

    // Load existing data (if any)
    sys.LoadAll();

    // -- Seed initial data if empty --------------------------------------------
    sep("SETUP: Seeding initial data");

    // Register users
    if (sys.RegisterUser("admin",   "admin123")) std::cout << "Registered admin\n";
    if (sys.RegisterUser("alice",   "pass1"))    std::cout << "Registered alice\n";
    if (sys.RegisterUser("bob",     "pass2"))    std::cout << "Registered bob\n";

    // Admin adds products
    User* admin = sys.Login("admin", "admin123");
    if (admin) {
        sys.AddProduct("Gaming Laptop",       "Electronics",  1299.99, 10);
        sys.AddProduct("Mechanical Keyboard", "Electronics",   89.99, 25);
        sys.AddProduct("USB-C Hub",           "Electronics",   35.00, 50);
        sys.AddProduct("Running Shoes",       "Sportswear",    59.99, 30);
        sys.AddProduct("Yoga Mat",            "Sportswear",    25.00, 40);
        sys.AddProduct("Protein Powder",      "Health",        49.99, 20);
        sys.AddProduct("Water Bottle",        "Health",        15.00, 60);
        sys.AddProduct("Desk Lamp",           "Furniture",     30.00, 35);
        sys.AddProduct("Office Chair",        "Furniture",    299.99, 8);
        sys.AddProduct("Wireless Mouse",      "Electronics",   45.00, 45);
    }

    // -- Customer: Alice -------------------------------------------------------
    sep("CUSTOMER: Alice browsing & shopping");

    User* alice = sys.Login("alice", "pass1");
    if (alice) {
        int uid = alice->GetId();

        // Browse catalog
        sys.BrowseProducts(uid);

        // View some products
        sys.ViewProduct(uid, 1);    // Gaming Laptop
        sys.ViewProduct(uid, 2);    // Mechanical Keyboard
        sys.ViewProduct(uid, 10);   // Wireless Mouse
        sys.ViewProduct(uid, 1);    // View laptop again

        // Add to cart
        sys.AddToCart(uid, 1, 1);   // 1x Gaming Laptop
        sys.AddToCart(uid, 2, 2);   // 2x Keyboard
        sys.AddToCart(uid, 10, 1);  // 1x Wireless Mouse
        sys.ViewCart(uid);

        // Update quantity
        sys.UpdateCartQty(uid, 2, 1);  // change keyboard to 1
        sys.ViewCart(uid);

        // Remove an item
        sys.RemoveFromCart(uid, 10);
        sys.ViewCart(uid);

        // Checkout
        sys.Checkout(uid);

        // Order history
        sys.ViewOrderHistory(uid);

        // Recommendations
        sys.GetRecommendations(uid, 5);
    }

    // -- Customer: Bob ---------------------------------------------------------
    sep("CUSTOMER: Bob browsing & shopping");

    User* bob = sys.Login("bob", "pass2");
    if (bob) {
        int uid = bob->GetId();

        sys.ViewProduct(uid, 4);    // Running Shoes
        sys.ViewProduct(uid, 5);    // Yoga Mat
        sys.ViewProduct(uid, 6);    // Protein Powder
        sys.ViewProduct(uid, 7);    // Water Bottle
        sys.ViewProduct(uid, 4);    // Shoes again

        sys.AddToCart(uid, 4, 1);   // Running Shoes
        sys.AddToCart(uid, 6, 2);   // Protein Powder x2
        sys.ViewCart(uid);
        sys.Checkout(uid);

        // Buy again
        sys.AddToCart(uid, 5, 1);
        sys.AddToCart(uid, 7, 3);
        sys.Checkout(uid);

        sys.ViewOrderHistory(uid);
        sys.GetRecommendations(uid, 5);
    }

    // -- Admin: Statistics & Management ----------------------------------------
    sep("ADMIN: System statistics");

    if (admin) {
        sys.ViewStatsMostViewed(5);
        sys.ViewStatsBestSelling(5);
        sys.ViewStatsActiveUsers(5);
        sys.ListAllProducts();
        sys.ListAllUsers();

        // Edit a product
        std::cout << "\n[Admin] Editing product 1...\n";
        sys.EditProduct(1, "Gaming Laptop Pro", "Electronics", 1499.99, 8);

        // Delete a product
        std::cout << "[Admin] Deleting product 8 (Desk Lamp)...\n";
        sys.DeleteProduct(8);

        sys.ListAllProducts();
    }

    // -- Save state ------------------------------------------------------------
    sep("Saving data to files");
    sys.SaveAll();
    std::cout << "Data saved to data/products.txt, users.txt, orders.txt\n";

    std::cout << "\nDemo complete.\n";
    return 0;
}
