#include <iostream>
#include <vector>
#include <string>
#include <memory> // 引入智能指针头文件

// ==========================================
// 类的定义保持不变 (精简展示)
// ==========================================
class Layer {
protected:
    std::string name_;
public:
    Layer(const std::string& name) : name_(name) {}
    virtual ~Layer() {
        std::cout << "[Layer Destructor] Destroying: " << name_ << std::endl;
    }
    virtual void forward() = 0;
};

class LinearLayer : public Layer {
private:
    float* weights_; // 这里为了展示析构效果保留裸指针，更好的做法是使用 std::vector
public:
    LinearLayer(const std::string& name, int in, int out) : Layer(name) {
        weights_ = new float[in * out];
        std::cout << "  [LinearLayer Constructor] Allocated weights for " << name_ << std::endl;
    }
    ~LinearLayer() override {
        delete[] weights_;
        std::cout << "  [LinearLayer Destructor] Freed weights for " << name_ << std::endl;
    }
    void forward() override {
        std::cout << ">>> Executing Linear Layer [" << name_ << "]" << std::endl;
    }
};

class ReLULayer : public Layer {
public:
    ReLULayer(const std::string& name) : Layer(name) {}
    ~ReLULayer() override {
        std::cout << "  [ReLULayer Destructor] Destroyed " << name_ << std::endl;
    }
    void forward() override {
        std::cout << ">>> Executing ReLU Layer [" << name_ << "]" << std::endl;
    }
};

// ==========================================
// 重构后的主程序：使用 std::unique_ptr
// ==========================================
int main() {
    std::cout << "--- Building Model Pipeline with Smart Pointers ---" << std::endl;

    // 核心变更 1：将 std::vector<Layer*> 替换为包含独占智能指针的容器
    // 这意味着 pipeline 容器独占了这些 Layer 对象的生命周期
    std::vector<std::unique_ptr<Layer>> pipeline;

    // 核心变更 2：使用 std::make_unique 创建对象，直接在容器内构建
    // 避免了任何显式的 new 关键字
    pipeline.push_back(std::make_unique<LinearLayer>("fc1", 128, 64));
    pipeline.push_back(std::make_unique<ReLULayer>("relu1"));
    pipeline.push_back(std::make_unique<LinearLayer>("fc2", 64, 10));

    std::cout << "\n--- Starting Forward Pass ---" << std::endl;
    
    // 核心变更 3：访问智能指针的方法与裸指针完全一致，重载了 -> 运算符
    for (size_t i = 0; i < pipeline.size(); ++i) {
        pipeline[i]->forward();
    }

    std::cout << "\n--- End of Main Scope ---" << std::endl;
    
    // 惊艳时刻：这里不再需要任何 for 循环去 delete 裸指针！
    // 当程序离开 main 函数的作用域时，pipeline 容器会被销毁。
    // 容器销毁会自动销毁它内部的每一个 std::unique_ptr。
    // std::unique_ptr 的析构函数会自动触发各个 Layer 的虚析构函数。

    return 0;
}