#include "shopping_cart.hpp"
#include "shopping_cart.cpp"

int main() {
    // 创建一个管理员对象
    Admin admin("admin", "114514");
    // 以刚才创建的管理员对象为管理员创建一个购物系统对象
    ShoppingSystem system(admin);
    // 运行购物系统
    system.run();
    
    return 0;
}
