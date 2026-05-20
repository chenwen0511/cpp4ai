#include <iostream>
#include <vector>
#include <string>

// ==========================================
// 1. 定义抽象基类 Layer
// ==========================================
class Layer {
protected:
    std::string name_;

public:
    Layer(const std::string& name) : name_(name) {}

    // 【极其关键】：虚析构函数！确保 delete 基类指针时，能正确调用派生类的析构函数
    virtual ~Layer() {
        std::cout << "[Layer Destructor] Destroying layer: " << name_ << std::endl;
    }

    // 纯虚函数，= 0 意味着 Layer 变成了抽象类 (不能被实例化)
    virtual void forward() = 0;

    // 普通成员函数
    std::string getName() const { return name_; }
};

// ==========================================
// 2. 派生类：LinearLayer (全连接层)
// ==========================================
class LinearLayer : public Layer {
private:
    int in_features_;
    int out_features_;
    float* weights_; // 模拟需要动态分配的权重内存

public:
    LinearLayer(const std::string& name, int in, int out) 
        : Layer(name), in_features_(in), out_features_(out) {
        
        // 构造函数中申请资源
        weights_ = new float[in_features_ * out_features_];
        std::cout << "  [LinearLayer Constructor] Allocated weights for " << name_ << std::endl;
    }

    // 重写析构函数，释放自己的资源
    ~LinearLayer() override {
        delete[] weights_;
        std::cout << "  [LinearLayer Destructor] Freed weights for " << name_ << std::endl;
    }

    // 重写 forward 函数，加上 override 关键字
    void forward() override {
        std::cout << ">>> Executing Linear Layer [" << name_ 
                  << "]: Matrix multiplication (" << in_features_ 
                  << " -> " << out_features_ << ")" << std::endl;
    }
};

// ==========================================
// 3. 派生类：ReLULayer (激活层)
// ==========================================
class ReLULayer : public Layer {
public:
    // 调用基类构造函数初始化 name_
    ReLULayer(const std::string& name) : Layer(name) {}

    ~ReLULayer() override {
        std::cout << "  [ReLULayer Destructor] Destroyed " << name_ << std::endl;
    }

    void forward() override {
        std::cout << ">>> Executing ReLU Layer [" << name_ 
                  << "]: max(0, x)" << std::endl;
    }
};

// ==========================================
// 4. 主程序：推理流水线调度
// ==========================================
int main() {
    std::cout << "--- Building Model Pipeline ---" << std::endl;

    // 核心：使用基类指针 (Layer*) 的容器，统一管理不同的派生类对象
    std::vector<Layer*> pipeline;

    // 在堆上动态实例化派生类，并存入流水线
    pipeline.push_back(new LinearLayer("fc1", 128, 64));
    pipeline.push_back(new ReLULayer("relu1"));
    pipeline.push_back(new LinearLayer("fc2", 64, 10));

    std::cout << "\n--- Starting Forward Pass ---" << std::endl;
    
    // 动态多态的魅力：无需知道具体的类型，只要调用 forward() 
    // vtable 会自动把调用路由到正确的子类实现
    for (size_t i = 0; i < pipeline.size(); ++i) {
        pipeline[i]->forward();
    }

    std::cout << "\n--- Cleaning Up Resources ---" << std::endl;
    
    // 因为使用了 new，必须手动 delete。
    // 这里也是测试虚析构函数是否工作的关键。
    for (size_t i = 0; i < pipeline.size(); ++i) {
        delete pipeline[i]; 
    }
    pipeline.clear();

    std::cout << "--- Program End ---" << std::endl;
    return 0;
}
