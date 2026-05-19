#include <iostream>
#include <stdexcept>
#include <cstring> // 用于 memcpy，提供极速的内存块拷贝

class Tensor {
private:
    float* data_;
    size_t size_;

public:
    // 1. 普通构造函数
    Tensor(size_t s) : size_(s), data_(new float[s]) {
        std::cout << "[Constructor] Allocated " << size_ << " floats." << std::endl;
        for (size_t i = 0; i < size_; ++i) data_[i] = 0.0f;
    }

    // 2. 析构函数 (Rule of 3 - Part 1)
    ~Tensor() {
        delete[] data_;
        std::cout << "[Destructor] Freed memory." << std::endl;
    }

    // ----------------- 本节课核心内容 -----------------

    // 3. 拷贝构造函数 (Rule of 3 - Part 2)
    // 触发场景: Tensor b = a; 或 Tensor b(a);
    Tensor(const Tensor& other) : size_(other.size_), data_(new float[other.size_]) {
        std::cout << "[Copy Constructor] Deep copying " << size_ << " floats." << std::endl;
        // 使用 memcpy 替代 for 循环，在底层具有更高的执行效率
        std::memcpy(data_, other.data_, size_ * sizeof(float));
    }

    // 4. 拷贝赋值运算符 (Rule of 3 - Part 3)
    // 触发场景: b = a; (此时 b 已经完成了构造并且拥有自己的内存)
    Tensor& operator=(const Tensor& other) {
        std::cout << "[Copy Assignment] Re-allocating and deeply copying." << std::endl;
        
        // 步骤 A: 自我赋值检查 (极度重要!)
        // 如果外部执行了 a = a;，跳过此检查会导致先把 a 的内存释放了，接下来就无数据可拷
        if (this == &other) {
            return *this;
        }

        // 步骤 B: 释放当前对象已经持有的旧资源
        delete[] data_;

        // 步骤 C: 重新根据源对象分配新内存
        size_ = other.size_;
        data_ = new float[size_];

        // 步骤 D: 深拷贝数据
        std::memcpy(data_, other.data_, size_ * sizeof(float));

        // 步骤 E: 返回自身的引用，以支持连续赋值操作 (如 a = b = c;)
        return *this;
    }

    // --------------------------------------------------

    // const 成员函数：承诺不修改内部状态
    size_t getSize() const { 
        return size_; 
    }

    // 修改内部状态，不能加 const
    void setValue(size_t index, float val) {
        if (index >= size_) throw std::out_of_range("Index out of bounds!");
        data_[index] = val;
    }

    // 打印只读数据，必须加 const
    void print() const {
        std::cout << "Tensor data: [ ";
        for (size_t i = 0; i < size_; ++i) {
            std::cout << data_[i] << " ";
        }
        std::cout << "]" << std::endl;
    }
};

int main() {
    std::cout << "--- 1. Testing Normal Construction ---" << std::endl;
    Tensor t1(3);
    t1.setValue(0, 1.1f);
    t1.setValue(1, 2.2f);
    t1.setValue(2, 3.3f);
    t1.print();

    std::cout << "\n--- 2. Testing Copy Constructor ---" << std::endl;
    // 此时 t2 刚刚被创建，触发 Copy Constructor
    Tensor t2 = t1; 
    
    // 修改 t2 的数据，验证深拷贝是否成功（t1 不应受影响）
    t2.setValue(0, 9.9f);
    std::cout << "t1 (Source): "; t1.print();
    std::cout << "t2 (Copied and Modified): "; t2.print();

    std::cout << "\n--- 3. Testing Copy Assignment ---" << std::endl;
    Tensor t3(5); // t3 最初有 5 个元素的空间
    
    // 此时 t3 已经存在，将触发 Copy Assignment Operator
    // 它会先释放自己的 5 个元素的空间，再申请 3 个元素的空间并拷贝 t1
    t3 = t1; 
    std::cout << "t3 (After Assignment): "; t3.print();

    std::cout << "\n--- 4. End of Scope (Destructors will trigger) ---" << std::endl;
    return 0;
}