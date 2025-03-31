#include "shopping_cart.hpp"

// Product 类的实现
// 构造函数, 初始化列表写法
Product::Product(string name, double price, string desc, int stock)
    : name(name), price(price), desc(desc), stock(stock) {}


// CartItem 类的实现
// 构造函数, 初始化购物车中的项目的名称和数量
CartItem::CartItem(string name, int qty) : name(name), quantity(qty) {}


// PurchaseRec 类的实现

// 构造函数, 初始化购买记录的名称、数量、价格和时间
PurchaseRec::PurchaseRec(string name, int qty, double price, time_t time)
    : name(name), quantity(qty), price(price), time(time) {}


// Cart 类的实现
// addItem：将商品添加到购物车，如果已存在则增加数量
void Cart::addItem(string name, int quantity)
{
    if(quantity)
    for (auto &item : items)
    {
        if (item.name == name)
        {
            item.quantity += quantity;
            return;
        }
    }
    items.emplace_back(name, quantity);
}

// removeItem：从购物车中移除指定名称的商品
void Cart::removeItem(string name)
{    
    for(auto it = items.begin(); it < items.end(); it++)
    {
        if(it->name == name)
        {
            items.erase(it);
            break;
        }
    }
}

// updateItem：更新购物车中指定名称商品的数量
void Cart::updateItem(string name, int quantity)
{
    for (auto &item : items)
    {
        if (item.name == name)
        {
            item.quantity = quantity;
            return;
        }
    }
}

// displayCart：显示购物车的内容
void Cart::displayCart() const
{
    if (items.empty())
    {
        cout << "购物车为空。" << endl;
        return;
    }
    cout << "购物车内容：" << endl;
    cout << "====================" << endl;
    for (const auto &item : items)
    {
        cout << "商品名称: " << item.name << ", 数量: " << item.quantity << endl;
    }
    cout << "====================" << endl;
}

// calcTotalPrice：计算购物车中所有商品的总价格
double Cart::calcTotalPrice(const unordered_map<string, Product> &products) const
{
    double total = 0;
    for (const auto &item : items)
    {
        auto it = products.find(item.name);
        if (it != products.end())
        {
            total += it->second.price * item.quantity;
        }
    }
    return total;
}

// checkout：结账并返回购买记录
vector<PurchaseRec> Cart::checkout(unordered_map<string, Product> &products, double discountRate, double couponValue, const vector<CartItem> &selectedItems)
{
    double total = 0;
    vector<PurchaseRec> purchaseRecords;
    time_t now = time(0);

    // 遍历选择的购物车中的项
    for (const auto &item : selectedItems)
    {
        // 从商品中是否找到这个项
        auto it = products.find(item.name);
        // 如果找到了并且库存大于等于要买的数量
        if (it != products.end() && it->second.stock >= item.quantity)
        {
            // 结算
            total += it->second.price * item.quantity;
            it->second.stock -= item.quantity;
            // 将其加入购买记录
            purchaseRecords.emplace_back(item.name, item.quantity, it->second.price * item.quantity, now);

            // 更新购物车中对应商品的数量
            for (auto &cartItem : items)
            {
                if (cartItem.name == item.name)
                {
                    cartItem.quantity -= item.quantity;
                    if (cartItem.quantity <= 0)
                    {
                        removeItem(cartItem.name);
                    }
                    break;
                }
            }
        }
    }

    // 计算折扣, 首先计算折扣倍率
    double discount = total * discountRate;
    total -= discount;
    // 再计算优惠券的减的钱
    total -= couponValue;
    if (total < 0) total = 0;
    

    cout << "结算商品的总价为: " << total << " (折扣: " << discount << ", 优惠券: " << couponValue << ")" << endl;
    // 计算实际付款的金额
    for (auto &rec : purchaseRecords)
    {
        rec.price -= rec.price * discountRate;
        // 简化处理, 对于每一项, 优惠券平均优惠了
        rec.price -= (couponValue / purchaseRecords.size());
    }

    return purchaseRecords;
}

// saveCart：将购物车内容保存到文件
void Cart::saveCart(const string &username) const
{
    ofstream file(username + "_cart.txt");
    for (const auto &item : items)
        file << item.name << " " << item.quantity << endl;
    
    file.close();
}

// loadCart：从文件加载购物车内容
void Cart::loadCart(const string &username)
{
    ifstream file(username + "_cart.txt");
    string name;
    int quantity;

    // 将文件中的内容添加到购物车数组中
    while (file >> name >> quantity) addItem(name, quantity);
    
    file.close();
}

// Admin 类的实现

// 构造函数, 用于预置管理员的用户名和密码
Admin::Admin(string uname, string pwd) : username(uname), password(pwd), discountRate(0) {}

// login：管理员登录验证
bool Admin::login(string uname, string pwd)
{
    return (username == uname && password == pwd);
}

// addProduct：添加新产品到产品列表
void Admin::addProduct(unordered_map<string, Product> &products, string name, double price, string desc, int stock)
{
    products[name] = Product(name, price, desc, stock);
    saveProducts(products);
}

// removeProduct：从产品列表中移除产品
void Admin::removeProduct(unordered_map<string, Product> &products, string name)
{
    products.erase(name);
    saveProducts(products);
}

// updateProduct：更新产品信息
void Admin::updateProduct(unordered_map<string, Product> &products, string oldName, string newName, double newPrice, string newDesc, int newStock)
{
    auto it = products.find(oldName);
    if (it != products.end())
    {
        products.erase(it);
        products[newName] = Product(newName, newPrice, newDesc, newStock);
        saveProducts(products);
        cout << "商品信息更新成功！" << endl;
    }
    else
    {
        cout << "错误：商品未找到。" << endl;
    }
}

// displayProducts：显示所有产品信息
void Admin::displayProducts(const unordered_map<string, Product> &products)
{
    cout << "商品列表：" << endl;
    cout << "====================" << endl;
    for (const auto &pair : products)
    {
        const auto &product = pair.second;
        cout << "名称: " << product.name << ", 价格: " << product.price
             << ", 描述: " << product.desc << ", 库存: " << product.stock << endl;
    }
    cout << "====================" << endl;
}

// setDiscountRate：设置折扣率
void Admin::setDiscountRate(double rate)
{
    discountRate = rate;
}

// getDiscountRate：获取当前折扣率
double Admin::getDiscountRate() const
{
    return discountRate;
}

// saveProducts：将产品信息保存到文件
void Admin::saveProducts(const unordered_map<string, Product> &products)
{
    ofstream file("products.txt");
    for (const auto &pair : products)
    {
        const auto &product = pair.second;
        file << product.name << " " << product.price << " " << product.desc << " " << product.stock << endl;
    }
    file.close();
}

// loadProducts：从文件加载产品信息
void Admin::loadProducts(unordered_map<string, Product> &products)
{
    ifstream file("products.txt");
    string name, desc;
    double price;
    int stock;
    while (file >> name >> price >> desc >> stock)
        products[name] = Product(name, price, desc, stock);
    
    file.close();
}


// Customer 类的实现

// 构造函数：初始化顾客的用户名和密码，并设置初始优惠券金额为 0
Customer::Customer(string uname, string pwd) : username(uname), password(pwd), couponValue(0) {}

// login：顾客登录验证
bool Customer::login(string uname, string pwd)
{
    return (username == uname && password == pwd);
}

// registerAccount：注册新顾客账户
void Customer::registerAccount(string uname, string pwd, unordered_map<string, string> &customerDB)
{
    customerDB[uname] = pwd;
    saveCustomers(customerDB);
}

// changePassword：更改顾客密码
void Customer::changePassword(string uname, string newPwd, unordered_map<string, string> &customerDB)
{
    customerDB[uname] = newPwd;
    saveCustomers(customerDB);
}

// updatePassword：更新顾客密码
void Customer::updatePassword(string newPwd)
{
    password = newPwd;
}

// addToCart：将商品添加到购物车
void Customer::addToCart(const unordered_map<string, Product> &products, string name, int quantity)
{
    // 找商品
    auto it = products.find(name);
    // 如果找到了并且要添加的数量小于等于商品的库存
    if (it != products.end() && it->second.stock >= quantity)
    {
        // 将商品添加到购物车
        cart.addItem(name, quantity);
        cout << "已将 " << quantity << " 个 " << name << " 添加到购物车。" << endl;
    }
    else
    {
        cout << "商品不可用或库存不足。" << endl;
    }
}

// displayCart：显示购物车内容
void Customer::displayCart() const
{
    cart.displayCart();
}

// removeFromCart：从购物车中移除商品
void Customer::removeFromCart(string name)
{
    cart.removeItem(name);
    cout << "已从购物车中移除 " << name << "。" << endl;
}

// updateCartItem：更新购物车中商品的数量
void Customer::updateCartItem(string name, int quantity)
{
    cart.updateItem(name, quantity);
    cout << "已更新购物车中 " << name << " 的数量为 " << quantity << "。" << endl;
}

// displayCartTotalPrice：显示购物车中所有商品的总价
void Customer::displayCartTotalPrice(const unordered_map<string, Product> &products) const
{
    double total = cart.calcTotalPrice(products);
    cout << "购物车中所有商品的总价为: " << total << endl;
}

// checkout：结账并更新购买历史
void Customer::checkout(unordered_map<string, Product> &products, double discountRate)
{
    vector<CartItem> selectedItems;

    // 显示购物车内容
    cart.displayCart();

    // 选择要结算的商品及其数量
    while (true)
    {
        cout << "请输入要结算的商品名称 (输入 'done' 完成选择): ";
        string name;
        cin >> name;
        if (name == "done")
            break;

        int quantity;
        cout << "请输入要结算的数量: ";
        cin >> quantity;

        auto it = find_if(cart.items.begin(), cart.items.end(),
                          [&name](const CartItem &item)
                          { return item.name == name; });

        if (it != cart.items.end() && quantity <= it->quantity)
        {
            selectedItems.emplace_back(name, quantity);
        }
        else
        {
            cout << "购物车中没有足够的 " << name << " 或该商品不存在。" << endl;
        }
    }

    if (selectedItems.empty())
    {
        cout << "没有选择任何商品进行结算。" << endl;
        return;
    }

    // 输入优惠券码
    cout << "请输入优惠券码: ";
    string couponCode;
    cin >> couponCode;

    double totalPrice = cart.calcTotalPrice(products);
    double couponValue = min(totalPrice, genRandCouponValue());
    vector<PurchaseRec> newRecords = cart.checkout(products, discountRate, couponValue, selectedItems);
    purchaseHistory.insert(purchaseHistory.end(), newRecords.begin(), newRecords.end());
    savePurchaseHistory();
    saveCart();
}

// genRandCouponValue：生成随机优惠券金额
double Customer::genRandCouponValue()
{
    srand(time(0));
    return (rand() % 100) / 100.0 * 50; // 随机生成0到50之间的值
}

// queryProduct：根据名称查询产品
void Customer::queryProduct(const unordered_map<string, Product> &products, string name)
{
    auto it = products.find(name);
    if (it != products.end())
    {
        const auto &product = it->second;
        cout << "名称: " << product.name << ", 价格: " << product.price
             << ", 描述: " << product.desc << ", 库存: " << product.stock << endl;
    }
    else
    {
        cout << "未找到商品: " << name << "。尝试推荐最接近的商品。" << endl;
        recommendProducts(products, name);
    }
}

// recommendProducts：根据关键词推荐产品
void Customer::recommendProducts(const unordered_map<string, Product> &products, string keyword)
{
    cout << "推荐商品：" << endl;
    cout << "====================" << endl;

    transform(keyword.begin(),keyword.end(),keyword.begin(),::tolower);
    // 遍历产品了
    for (const auto &pair : products)
    {
        const auto &product = pair.second;
        string pname = pair.second.name;
        transform(pname.begin(), pname.end(), pname.begin(), ::tolower);
        // 如果能找到关键词就输出
        if (pname.find(keyword) != string::npos)
        {
            cout << "名称: " << product.name << ", 价格: " << product.price
                 << ", 描述: " << product.desc << ", 库存: " << product.stock << endl;
        }
    }
    cout << "====================" << endl;
}

// displayProducts：显示所有产品信息
void Customer::displayProducts(const unordered_map<string, Product> &products)
{
    cout << "商品列表：" << endl;
    cout << "====================" << endl;
    for (const auto &pair : products)
    {
        const auto &product = pair.second;
        cout << "名称: " << product.name << ", 价格: " << product.price
             << ", 描述: " << product.desc << ", 库存: " << product.stock << endl;
    }
    cout << "====================" << endl;
}

// saveCustomers：将顾客信息保存到文件
void Customer::saveCustomers(const unordered_map<string, string> &customerDB)
{
    ofstream file("customers.txt");
    for (const auto &[uname, pwd] : customerDB)
    {
        file << uname << " " << pwd << endl;
    }
    file.close();
}

// loadCustomers：从文件加载顾客信息
void Customer::loadCustomers(unordered_map<string, string> &customerDB)
{
    ifstream file("customers.txt");
    string uname, pwd;
    while (file >> uname >> pwd)
    {
        customerDB[uname] = pwd;
    }
    file.close();
}

// saveCart：将购物车保存到文件
void Customer::saveCart() const
{
    cart.saveCart(username);
}

// loadCart：从文件加载购物车
void Customer::loadCart()
{
    cart.loadCart(username);
}

// savePurchaseHistory：将购买历史保存到文件
void Customer::savePurchaseHistory() const
{
    ofstream file(username + "_history.txt");
    for (const auto &record : purchaseHistory)
    {
        file << record.name << " " << record.quantity << " " << record.price << " " << record.time << endl;
    }
    file.close();
}

// loadPurchaseHistory：从文件加载购买历史
void Customer::loadPurchaseHistory()
{
    ifstream file(username + "_history.txt");
    string name;
    int quantity;
    double price;
    time_t time;
    while (file >> name >> quantity >> price >> time)
    {
        purchaseHistory.emplace_back(name, quantity, price, time);
    }
    file.close();
}

// analyzePurchaseHistory：分析顾客在指定时间段内的购买历史
void Customer::analyzePurchaseHistory(time_t start, time_t end, const unordered_map<string, Product> &products) const
{
    unordered_map<string, double> descriptionTotal;
    // 遍历购买历史, 在时间段内的算进去
    for (const auto &record : purchaseHistory)
    {
        if (record.time >= start && record.time <= end)
        {
            auto it = products.find(record.name);
            if (it != products.end())
                descriptionTotal[it->second.desc] += record.price;
        }
    }
    cout << "购买历史分析：" << endl;
    cout << "====================" << endl;
    for (const auto &[desc, total] : descriptionTotal)
    {
        cout << "商品描述: " << desc << ", 购买金额: " << total << endl;
    }
    cout << "====================" << endl;
}

// getUsername：获取顾客用户名
string Customer::getUsername() const
{
    return username;
}

// getPurchaseHistory：获取顾客的购买历史
const vector<PurchaseRec> &Customer::getPurchaseHistory() const
{
    return purchaseHistory;
}

// validatePassword：验证密码是否符合要求
bool Customer::validatePassword(const string &pwd) const
{
    return password == pwd;
}


// ShoppingSystem 类的实现
// 构造函数：初始化购物系统, 这里创建一个 Customer 对象来初始化  currentCustomer
ShoppingSystem::ShoppingSystem(const Admin &a) : admin(a), isAdminLoggedIn(false), isCustomerLoggedIn(false), currentCustomer(Customer("", ""))
{
    // 商品数据和用户数据
    Admin::loadProducts(products);
    Customer::loadCustomers(customerDB);
}

// run：运行购物系统
void ShoppingSystem::run()
{
    int choice;
    while (true)
    {
        displayMenu();
        choice = getValidChoice(1, 6);

        switch (choice)
        {
        case 1:
            adminLogin();
            break;
        case 2:
            customerLogin();
            break;
        case 3:
            customerRegister();
            break;
        case 4:
            queryProducts();
            break;
        case 5:
            cout << "感谢使用购物系统！" << endl;
            return;
        }
    }
}

// adminLogin：管理员登录
void ShoppingSystem::adminLogin()
{
    string uname, pwd;
    cout << "请输入管理员用户名: ";
    cin >> uname;
    cout << "请输入管理员密码: ";
    cin >> pwd;

    if (admin.login(uname, pwd))
    {
        isAdminLoggedIn = true;
        cout << "管理员登录成功。\n";
        handleAdminActions();
    }
    else cout << "管理员账户或密码错误。\n";
}

// customerLogin：顾客登录
void ShoppingSystem::customerLogin()
{
    string uname, pwd;
    cout << "请输入顾客用户名: ";
    cin >> uname;
    cout << "请输入顾客密码: ";
    cin >> pwd;

    // 如果用户数据库中有这个账户并且密码正确
    if (customerDB.find(uname) != customerDB.end() && customerDB[uname] == pwd)
    {
        isCustomerLoggedIn = true;
        // 将已登录用户设置为当前用户
        currentCustomer = Customer(uname, pwd);
        // 加载购物车和购物历史记录
        currentCustomer.loadCart();
        currentCustomer.loadPurchaseHistory();
        cout << "顾客登录成功。\n";
        handleCustomerActions();
    }
    else cout << "顾客账户或密码错误。\n";
}

// customerRegister：顾客注册
void ShoppingSystem::customerRegister()
{
    string uname, pwd;
    cout << "请输入注册用户名: ";
    cin >> uname;
    cout << "请输入注册密码: ";
    cin >> pwd;

    // 如果当前用户数据库中没有这个用户名
    if (customerDB.find(uname) == customerDB.end())
    {
        Customer customer(uname, pwd);
        customer.registerAccount(uname, pwd, customerDB);
        cout << "顾客注册成功。\n";
    }
    else cout << "用户名已存在。\n";
}

// customerChangePassword：顾客修改密码
void ShoppingSystem::customerChangePassword()
{
    string oldPwd, newPwd;
    cout << "请输入旧密码: ";
    cin >> oldPwd;

    if (!currentCustomer.validatePassword(oldPwd))
    {
        cout << "旧密码错误。\n";
        return;
    }

    cout << "请输入新密码: ";
    cin >> newPwd;
    
    // 更新 currentCustomer 这个对象的密码
    currentCustomer.updatePassword(newPwd);
    // 将新密码保存到数据库和文件中
    currentCustomer.changePassword(currentCustomer.getUsername(), newPwd, customerDB);
    cout << "密码修改成功。\n";
}

// queryProducts：查询产品, 疑似有点答辩了, 前面 customer 里面也有一个, 属于是屎山了
void ShoppingSystem::queryProducts()
{
    int queryChoice;
    cout << "1. 查询所有商品\n2. 查询特定商品\n";
    queryChoice = getValidChoice(1, 2);

    if (queryChoice == 1)
    {
        displayProducts(products);
    }
    else if (queryChoice == 2)
    {
        string name;
        cout << "请输入商品名称或关键字: ";
        cin >> name;
        currentCustomer.queryProduct(products, name);
    }
}

// displayMenu：显示系统菜单
void ShoppingSystem::displayMenu()
{
    cout << "\n======== 购物系统菜单 ========" << endl;
    cout << "1. 管理员登录" << endl;
    cout << "2. 顾客登录" << endl;
    cout << "3. 顾客注册" << endl;
    cout << "4. 查询商品" << endl;
    cout << "5. 退出" << endl;
    cout << "==============================" << endl;
    cout << "请选择一个选项: ";
}

// handleAdminActions：处理管理员操作
void ShoppingSystem::handleAdminActions()
{
    int adminChoice;
    while (isAdminLoggedIn)
    {
        cout << "\n======= 管理员操作菜单 ========" << endl;
        cout << "1. 添加商品" << endl;
        cout << "2. 移除商品" << endl;
        cout << "3. 修改商品信息" << endl;
        cout << "4. 显示商品" << endl;
        cout << "5. 设置折扣活动" << endl;
        cout << "6. 退出" << endl;
        cout << "==============================" << endl;
        cout << "请选择一个选项: ";
        adminChoice = getValidChoice(1, 6);

        if (adminChoice == 1)
        {
            string name, desc;
            double price;
            int stock;
            cout << "请输入商品名称: ";
            cin >> name;
            cout << "请输入商品价格: ";
            cin >> price;
            cout << "请输入商品描述: ";
            cin.ignore();
            getline(cin, desc);
            cout << "请输入商品库存: ";
            cin >> stock;
            // 添加商品
            admin.addProduct(products, name, price, desc, stock);
            cout << "商品添加成功！" << endl;
        }
        else if (adminChoice == 2)
        {
            string name;
            cout << "请输入要移除的商品名称: ";
            cin >> name;
            // 移除商品
            admin.removeProduct(products, name);
            cout << "商品移除成功！" << endl;
        }
        else if (adminChoice == 3)
        {
            string oldName, newName, newDesc;
            double newPrice;
            int newStock;
            cout << "请输入要修改的商品名称: ";
            cin >> oldName;
            cout << "请输入新的商品名称: ";
            cin >> newName;
            cout << "请输入新的商品价格: ";
            cin >> newPrice;
            cout << "请输入新的商品描述: ";
            cin.ignore();
            getline(cin, newDesc);
            cout << "请输入新的商品库存: ";
            cin >> newStock;
            // 更新商品信息
            admin.updateProduct(products, oldName, newName, newPrice, newDesc, newStock);
        }
        else if (adminChoice == 4)
        {
            admin.displayProducts(products);
        }
        else if (adminChoice == 5)
        {
            double rate;
            cout << "请输入折扣率（0到1之间）: ";
            cin >> rate;
            // 设置折扣率
            admin.setDiscountRate(rate);
            cout << "折扣活动已设置。\n";
        }
        else if (adminChoice == 6)
        {
            isAdminLoggedIn = false;
            cout << "管理员已退出登录。\n";
        }
    }
}

// handleCustomerActions：处理顾客操作
void ShoppingSystem::handleCustomerActions()
{
    int customerChoice;
    while (isCustomerLoggedIn)
    {
        cout << "\n======= 顾客操作菜单 ========" << endl;
        cout << "1. 修改密码" << endl;
        cout << "2. 显示购物车" << endl;
        cout << "3. 添加商品到购物车" << endl;
        cout << "4. 从购物车中移除商品" << endl;
        cout << "5. 修改购物车商品数量" << endl;
        cout << "6. 显示购物车总价" << endl;
        cout << "7. 结算购物车商品" << endl;
        cout << "8. 查看购物历史" << endl;
        cout << "9. 查询商品" << endl;
        cout << "10. 购物数据分析" << endl;
        cout << "11. 退出" << endl;
        cout << "=============================" << endl;
        cout << "请选择一个选项: ";
        customerChoice = getValidChoice(1, 11);

        if (customerChoice == 1)
        {
            // 修改密码
            customerChangePassword();
        }
        else if (customerChoice == 2)
        {
            // 展示购物车
            currentCustomer.displayCart();
        }
        else if (customerChoice == 3)
        {
            string name;
            int quantity;
            cout << "请输入商品名称: ";
            cin >> name;
            cout << "请输入添加数量: ";
            cin >> quantity;
            // 添加商品到购物车
            currentCustomer.addToCart(products, name, quantity);
            currentCustomer.saveCart();
        }
        else if (customerChoice == 4)
        {
            string name;
            cout << "请输入商品名称: ";
            cin >> name;
            // 从购物车移除商品
            currentCustomer.removeFromCart(name);
            currentCustomer.saveCart();
        }
        else if (customerChoice == 5)
        {
            string name;
            int quantity;
            cout << "请输入商品名称: ";
            cin >> name;
            cout << "请输入新数量: ";
            quantity = getValidChoice(1, products[name].stock);

            // 更新购物车中商品的数量
            currentCustomer.updateCartItem(name, quantity);
            currentCustomer.saveCart();
        }
        else if (customerChoice == 6)
        {
            // 展示购物车中所有商品的价格综合
            currentCustomer.displayCartTotalPrice(products);
        }
        else if (customerChoice == 7)
        {
            // 结算
            currentCustomer.checkout(products, admin.getDiscountRate());
        }
        else if (customerChoice == 8)
        {
            // 查询购物历史
            if(currentCustomer.getPurchaseHistory().empty())
            {
                cout << "您没有购物历史记录";
                continue;
            }

            for (const auto &record : currentCustomer.getPurchaseHistory())
            {
                // ptm 是一个指向 tm 结构的指针 
                tm *ptm = localtime(&record.time);
                char buffer[32];
                // strftime 用于根据指定的格式将时间结构格式化为字符串
                strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
                cout << "商品名称: " << record.name << ", 数量: " << record.quantity << ", 实际支付价格: " << record.price << ", 购买时间: " << buffer << endl;
            }
        }
        else if (customerChoice == 9)
        {
            // 查询商品信息
            queryProducts();
        }
        else if (customerChoice == 10)
        {
            string startDateStr, endDateStr;
            cout << "请输入开始日期 (YYYY-MM-DD): ";
            cin >> startDateStr;
            cout << "请输入结束日期 (YYYY-MM-DD): ";
            cin >> endDateStr;

            tm tmStart = {};
            // stringstream 用于在字符串和其他数据类型之间进行流式输入输出操作
            // 创建一个 stringstream 对象 ssStart，并将 startDateStr 初始化为其内容
            stringstream ssStart(startDateStr);
            // 从 stringstream 流中提取字符串, 解析字符串并将结果存储在 tmStart 中
            // get_time 用于解析字符串
            ssStart >> get_time(&tmStart, "%Y-%m-%d");
            time_t startTime = mktime(&tmStart);

            tm tmEnd = {};
            stringstream ssEnd(endDateStr);
            ssEnd >> get_time(&tmEnd, "%Y-%m-%d");
            time_t endTime = mktime(&tmEnd);

            currentCustomer.analyzePurchaseHistory(startTime, endTime, products);
        }
        else if (customerChoice == 11)
        {
            isCustomerLoggedIn = false;
            currentCustomer.saveCart();
            cout << "顾客已退出登录。\n";
        }
    }
}

// displayProducts：显示所有产品信息
void ShoppingSystem::displayProducts(const unordered_map<string, Product> &products)
{
    cout << "商品列表：" << endl;
    cout << "====================" << endl;
    for (const auto &pair : products)
    {
        const auto &product = pair.second;
        cout << "名称: " << product.name << ", 价格: " << product.price
             << ", 描述: " << product.desc << ", 库存: " << product.stock << endl;
    }
    cout << "====================" << endl;
}

// getValidChoice：获取有效的用户选择
int ShoppingSystem::getValidChoice(int min, int max)
{
    int choice;
    while (true)
    {
        cin >> choice;
        if (cin.fail() || choice < min || choice > max)
        {
            // 重置 fail 状态
            cin.clear();
            // numeric_limits<streamsize>::max() 返回 streamsize 类型所能表示的最大值
            // 这里表示忽略输入流中的所有字符，直到遇到换行符
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "输入无效，请输入一个有效的选项 (" << min << "-" << max << "): ";
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
    }
}
