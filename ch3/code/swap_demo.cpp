#include <iostream>

// 1. 传值（错误示范）：这只是交换了函数内部拷贝的副本
void swap_val(int x, int y) {
    int temp = x;
    x = y;
    y = temp;
}

// 2. 传指针：通过内存地址直接操作原数据
void swap_ptr(int* x, int* y) {
    int temp = *x; // 读取 x 指向的值
    *x = *y;       // 将 y 指向的值赋给 x 指向的内存
    *y = temp;
}

// 3. 传引用：最优雅、最现代的C++写法
void swap_ref(int& x, int& y) {
    int temp = x;  // 就像操作原变量一样
    x = y;
    y = temp;
}

int main() {
    int a = 1, b = 2;
    swap_val(a, b);
    std::cout << "After Pass by Value: a=" << a << ", b=" << b << std::endl; // 失败，依然是 1, 2

    swap_ptr(&a, &b); // 需要显式传入地址
    std::cout << "After Pass by Pointer: a=" << a << ", b=" << b << std::endl; // 成功，变成了 2, 1

    swap_ref(a, b);   // 语法看起来和传值一样，但底层传递了引用
    std::cout << "After Pass by Reference: a=" << a << ", b=" << b << std::endl; // 再次成功，变回 1, 2

    return 0;
}