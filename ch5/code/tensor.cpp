#include <iostream>
#include <stdexcept>

class Tensor {
private:
    // 隐藏内部细节，防止外部随意修改指针或大小
    float* data_;
    size_t size_;

public:
    // 1. 构造函数与成员初始化列表
    // 负责在堆上分配资源
    Tensor(size_t s) : size_(s), data_(new float[s]) {
        std::cout << "[Constructor] Tensor created, allocated " 
                  << size_ * sizeof(float) << " bytes on Heap." << std::endl;
        // 简单初始化为 0
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = 0.0f;
        }
    }

    // 2. 析构函数 (名称前加 ~)
    // 负责在对象生命周期结束时清理资源
    ~Tensor() {
        delete[] data_; // 释放堆内存
        std::cout << "[Destructor] Tensor destroyed, memory freed." << std::endl;
    }

    // 3. 公开的接口：获取大小
    size_t getSize() const { 
        return size_; 
    }

    // 4. 公开的接口：设置数据
    void setValue(size_t index, float val) {
        if (index >= size_) {
            throw std::out_of_range("Index out of bounds!");
        }
        data_[index] = val;
    }

    // 5. 公开的接口：打印数据
    void print() const {
        std::cout << "Tensor data: [ ";
        for (size_t i = 0; i < size_; ++i) {
            std::cout << data_[i] << " ";
        }
        std::cout << "]" << std::endl;
    }

    // 6. 公开的接口：简单的原地加法 (将另一个 Tensor 的数据加到自己身上)
    // 注意这里按常量引用传递 (const Tensor&)，避免昂贵的拷贝！
    void addInplace(const Tensor& other) {
        if (this->size_ != other.size_) {
            throw std::invalid_argument("Tensor sizes must match for addition!");
        }
        for (size_t i = 0; i < size_; ++i) {
            // this 是指向当前对象的指针
            this->data_[i] += other.data_[i]; 
        }
    }
};

int main() {
    std::cout << "--- Program Start ---" << std::endl;

    {   // 使用大括号创建一个局部作用域
        std::cout << "Entering local scope..." << std::endl;

        // 在栈上创建 Tensor 对象，底层会自动在堆上分配数组
        Tensor t1(5); 
        Tensor t2(5);

        // 初始化数据
        for (size_t i = 0; i < t1.getSize(); ++i) {
            t1.setValue(i, 1.0f);
            t2.setValue(i, i * 2.0f); // 0, 2, 4, 6, 8
        }

        std::cout << "Before addition:" << std::endl;
        t1.print();

        // 执行原地加法
        t1.addInplace(t2);

        std::cout << "After addition:" << std::endl;
        t1.print();

        std::cout << "Leaving local scope..." << std::endl;
    } // 离开作用域，t1 和 t2 将自动调用析构函数，安全释放堆内存！

    std::cout << "--- Program End ---" << std::endl;
    return 0;
}