#include <iostream>
#include <vector>
#include <chrono>

class Tensor {
private:
    float* data_;
    size_t size_;

public:
    // 1. 普通构造
    Tensor(size_t s) : size_(s), data_(new float[s]) {}

    // 2. 析构
    ~Tensor() {
        delete[] data_; // 如果 data_ 是 nullptr，delete[] 是安全的无操作
    }

    // 3. 拷贝构造 (深拷贝) - 模拟极大的性能开销
    Tensor(const Tensor& other) : size_(other.size_), data_(new float[other.size_]) {
        for(size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
    }

    // ----------------- 本节核心：移动构造 -----------------
    // 参数是右值引用 Tensor&&，代表 other 即将被销毁
    Tensor(Tensor&& other) noexcept 
        : size_(other.size_), data_(other.data_) { // 步骤 A: 窃取指针
        
        // 步骤 B: 将源对象的指针置空！极度重要！
        // 否则 other 析构时，会把我们刚偷来的内存释放掉 (Double Free)
        other.size_ = 0;
        other.data_ = nullptr; 
    }
    // ------------------------------------------------------

    // 防止编译器优化干扰我们的测试（禁用 RVO 优化，强制走构造函数）
    // Tensor(const Tensor&) = delete; // 实验中我们可以暂时删除或保留拷贝构造，这里用代码逻辑演示
};

// 模拟一个复杂的算子，生成并返回一个巨大的特征图 (例如 100MB)
// 注意：返回类型是具体对象，而不是指针或引用
Tensor generateHugeFeatureMap() {
    Tensor temp(25000000); // 2500万个 float，约 100MB
    return temp; 
    // 当 return temp 时，temp 变成了一个右值（临时对象）
    // 如果没有移动构造函数，编译器将被迫执行耗时的深拷贝
    // 如果有移动构造函数，底层指针瞬间转移！
}

int main() {
    std::cout << "Starting generation..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 这里的赋值操作，完美触发了移动构造函数
    Tensor my_tensor = generateHugeFeatureMap(); 
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    
    std::cout << "Time taken to return 100MB Tensor: " << elapsed.count() << " ms" << std::endl;
    
    return 0;
}