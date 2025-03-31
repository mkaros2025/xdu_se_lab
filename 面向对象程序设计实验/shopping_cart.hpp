#ifndef SHOPPING_CART_HPP
#define SHOPPING_CART_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <limits>

using namespace std;

// 商品类
class Product
{
public:
    string name;  // 商品名称
    double price; // 商品价格
    string desc;  // 商品描述
    int stock;    // 商品库存数量

    // 构造函数，用于初始化商品详情
    Product(string name = "", double price = 0, string desc = "", int stock = 0);
};

// CartItem 类表示购物车中的项目，包含名称和数量。
class CartItem
{
public:
    string name;  // 购物车中商品的名称
    int quantity; // 购物车中商品的数量

    // 构造函数，用于初始化购物车项目详情
    CartItem(string name, int qty);
};

// PurchaseRec 类表示购买记录，包含商品名称、数量、价格和购买时间。
class PurchaseRec
{
public:
    string name;  // 已购买商品的名称
    int quantity; // 已购买商品的数量
    double price; // 已购买商品的价格
    time_t time;  // 购买时间

    // 构造函数，用于初始化购买记录详情
    PurchaseRec(string name, int qty, double price, time_t time);
};

// Cart 类表示购物车，包含一系列项目和各种操作。
class Cart
{
public:
    vector<CartItem> items; // 购物车中的项目列表

    // 将项目添加到购物车
    void addItem(string name, int quantity);

    // 从购物车中移除项目
    void removeItem(string name);

    // 更新购物车中项目的数量
    void updateItem(string name, int quantity);

    // 显示购物车内容
    void displayCart() const;

    // 计算购物车中所有项目的总价格
    double calcTotalPrice(const unordered_map<string, Product> &products) const;

    // 结账并返回购买记录列表
    vector<PurchaseRec> checkout(unordered_map<string, Product> &products, double discountRate, double couponValue, const vector<CartItem> &selectedItems);

    // 将购物车保存到文件
    void saveCart(const string &username) const;

    // 从文件加载购物车
    void loadCart(const string &username);
};

// Admin 类表示管理员，包含用户名、密码和折扣率管理功能。
class Admin
{
private:
    string username;     // 管理员用户名
    string password;     // 管理员密码
    double discountRate; // 管理员设置的折扣率

public:
    // 构造函数，用于预置管理员账号密码
    Admin(string uname, string pwd);

    // 管理员登录
    bool login(string uname, string pwd);

    // 向商品列表中添加商品
    void addProduct(unordered_map<string, Product> &products, string name, double price, string desc, int stock);

    // 从商品列表中移除商品
    void removeProduct(unordered_map<string, Product> &products, string name);

    // 更新商品列表中的商品
    void updateProduct(unordered_map<string, Product> &products, string oldName, string newName, double newPrice, string newDesc, int newStock);

    // 显示所有商品
    void displayProducts(const unordered_map<string, Product> &products);

    // 设置折扣率
    void setDiscountRate(double rate);

    // 获取当前折扣率
    double getDiscountRate() const;

    // 静态方法：将商品保存到文件
    static void saveProducts(const unordered_map<string, Product> &products);

    // 静态方法：从文件加载商品
    static void loadProducts(unordered_map<string, Product> &products);
};

// Customer 类表示顾客，包含用户名、密码、购物车和购买历史。
class Customer
{
private:
    string username;                     // 顾客用户名
    string password;                     // 顾客密码
    Cart cart;                           // 顾客的购物车
    double couponValue;                  // 顾客可用的优惠券金额
    vector<PurchaseRec> purchaseHistory; // 顾客的购买历史

public:
    // 构造函数，用于初始化顾客用户名和密码
    Customer(string uname, string pwd);

    // 顾客登录
    bool login(string uname, string pwd);

    // 注册新顾客账户
    void registerAccount(string uname, string pwd, unordered_map<string, string> &customerDB);

    // 更改顾客密码
    void changePassword(string uname, string newPwd, unordered_map<string, string> &customerDB);

    // 更新顾客密码
    void updatePassword(string newPwd);

    // 将项目添加到顾客的购物车
    void addToCart(const unordered_map<string, Product> &products, string name, int quantity);

    // 显示顾客购物车的内容
    void displayCart() const;

    // 从顾客的购物车中移除项目
    void removeFromCart(string name);

    // 更新顾客购物车中项目的数量
    void updateCartItem(string name, int quantity);

    // 显示顾客购物车中所有项目的总价格
    void displayCartTotalPrice(const unordered_map<string, Product> &products) const;

    // 结账并更新购买历史
    void checkout(unordered_map<string, Product> &products, double discountRate);

    // 生成随机优惠券金额
    double genRandCouponValue();

    // 根据名称查询商品
    void queryProduct(const unordered_map<string, Product> &products, string name);

    // 根据关键词推荐商品
    void recommendProducts(const unordered_map<string, Product> &products, string keyword);

    // 查询所有商品
    void queryProducts(const unordered_map<string, Product> &products);

    // 静态方法：显示所有商品
    static void displayProducts(const unordered_map<string, Product> &products);

    // 静态方法：将顾客数据保存到文件
    static void saveCustomers(const unordered_map<string, string> &customerDB);

    // 静态方法：从文件加载顾客数据
    static void loadCustomers(unordered_map<string, string> &customerDB);

    // 将顾客的购物车保存到文件
    void saveCart() const;

    // 从文件加载顾客的购物车
    void loadCart();

    // 将顾客的购买历史保存到文件
    void savePurchaseHistory() const;

    // 从文件加载顾客的购买历史
    void loadPurchaseHistory();

    // 分析顾客在指定时间段内的购买历史
    void analyzePurchaseHistory(time_t start, time_t end, const unordered_map<string, Product> &products) const;

    // 获取顾客用户名
    string getUsername() const;

    // 获取顾客的购买历史
    const vector<PurchaseRec> &getPurchaseHistory() const;

    // 验证密码是否符合要求
    bool validatePassword(const string &pwd) const;
};

// ShoppingSystem 类表示购物系统，包含管理员、商品列表、顾客数据库和系统操作。
class ShoppingSystem
{
private:
    Admin admin;                              // 系统管理员
    unordered_map<string, Product> products;  // 商品列表
    unordered_map<string, string> customerDB; // 顾客数据库
    bool isAdminLoggedIn;                     // 管理员登录状态
    bool isCustomerLoggedIn;                  // 顾客登录状态
    Customer currentCustomer;                 // 当前登录的顾客

public:
    // 构造函数，用于初始化购物系统
    ShoppingSystem(const Admin &a);

    // 运行购物系统
    void run();

    // 管理员登录
    void adminLogin();

    // 顾客登录
    void customerLogin();

    // 顾客注册
    void customerRegister();

    // 顾客修改密码
    void customerChangePassword();

    // 查询商品
    void queryProducts();

    // 显示菜单
    void displayMenu();

    // 处理管理员操作
    void handleAdminActions();

    // 处理顾客操作
    void handleCustomerActions();

    // 静态方法：显示所有商品
    static void displayProducts(const unordered_map<string, Product> &products);

    // 静态方法：获取有效的用户选择
    static int getValidChoice(int min, int max);
};

#endif
