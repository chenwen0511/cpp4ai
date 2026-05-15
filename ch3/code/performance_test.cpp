#include <iostream>
#include <vector>
#include <chrono> // 用于高精度计时

// 定义一个别名，代表百万级别的浮点数组
using LargeTensor = std::vector<float>;

// 1. 按值传递：每次调用都会触发底层数据的完整深拷贝
float process_by_value(LargeTensor tensor) {
    float sum = 0;
    // 假设进行一些简单的求和计算
    for(size_t i = 0; i < tensor.size(); ++i) {
        sum += tensor[i];
    }
    return sum;
}

// 2. 按常量引用传递：不发生拷贝，直接读取原内存，且保证不被修改
float process_by_reference(const LargeTensor& tensor) {
    float sum = 0;
    for(size_t i = 0; i < tensor.size(); ++i) {
        sum += tensor[i];
    }
    return sum;
}

int main() {
    std::cout << "Allocating memory for 100,000,000 floats..." << std::endl;
    // 初始化一个包含 1亿个浮点数的大型数组（约 400MB 内存）
    LargeTensor big_tensor(100000000, 1.0f); 

    // --- 测试传值耗时 ---
    auto start_val = std::chrono::high_resolution_clock::now();
    float result_val = process_by_value(big_tensor);
    auto end_val = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> diff_val = end_val - start_val;
    std::cout << "[Pass by Value] Time taken: " << diff_val.count() << " ms" << std::endl;

    // --- 测试传引用耗时 ---
    auto start_ref = std::chrono::high_resolution_clock::now();
    float result_ref = process_by_reference(big_tensor);
    auto end_ref = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> diff_ref = end_ref - start_ref;
    std::cout << "[Pass by Reference] Time taken: " << diff_ref.count() << " ms" << std::endl;

    return 0;
}
