#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <functional>

// ==========================================
// 1. 抽象基类：所有算子的接口契约
// ==========================================
class Operator {
public:
    virtual ~Operator() = default; // 必须是虚析构函数，保证多态释放安全
    virtual void forward() = 0;    // 纯虚函数，定义前向传播接口
};

// ==========================================
// 2. 具体算子实现
// ==========================================
class Conv2D : public Operator {
public:
    void forward() override {
        std::cout << "[Engine] Executing Conv2D... computing spatial convolution." << std::endl;
    }
};

class ReLU : public Operator {
public:
    void forward() override {
        std::cout << "[Engine] Executing ReLU... applying max(0, x)." << std::endl;
    }
};

// ==========================================
// 3. 核心：算子注册表与工厂类
// ==========================================
class OpFactory {
private:
    // 定义一个类型别名：CreatorFunc 是一个函数，没有参数，返回一个 Operator 的独占智能指针
    using CreatorFunc = std::function<std::unique_ptr<Operator>()>;
    
    // 注册表：将字符串映射到具体的创建函数
    std::map<std::string, CreatorFunc> registry_;

public:
    // 注册函数：向字典中添加算子
    void registerOp(const std::string& name, CreatorFunc func) {
        registry_[name] = func;
        std::cout << "[Factory] Operator registered: " << name << std::endl;
    }

    // 创建函数：根据字符串名字，动态生成智能指针
    std::unique_ptr<Operator> createOp(const std::string& name) {
        auto it = registry_.find(name);
        if (it != registry_.end()) {
            // it->second 就是那个创建函数，调用它 () 即可生成对象
            return it->second(); 
        }
        std::cerr << "[Factory] Error: Operator '" << name << "' not found!" << std::endl;
        return nullptr;
    }
};

// ==========================================
// 4. 对外提供的 C 风格 API，用于封装编译成 .so 库
// ==========================================
// extern "C" 告诉编译器不要对函数名进行 C++ Name Mangling（名字粉碎），
// 确保 Python 的 ctypes 或其他 C 语言程序可以直接加载这个库。
extern "C" {
    void run_mock_inference() {
        std::cout << "\n--- Engine Initialization ---" << std::endl;
        OpFactory factory;

        // 模拟框架启动时的算子注册阶段
        // 使用 Lambda 表达式封装对象的创建逻辑
        factory.registerOp("Conv2D", []() { return std::make_unique<Conv2D>(); });
        factory.registerOp("ReLU", []() { return std::make_unique<ReLU>(); });

        std::cout << "\n--- Parsing Model Graph (Simulated) ---" << std::endl;
        // 假设我们从模型文件 (如 JSON 或 ONNX) 中解析出了以下算子顺序
        std::string model_nodes[] = {"Conv2D", "ReLU", "UnknownOp", "Conv2D"};

        std::cout << "\n--- Execution Phase ---" << std::endl;
        for (const auto& node_name : model_nodes) {
            // 根据字符串动态创建对象
            std::unique_ptr<Operator> op = factory.createOp(node_name);
            if (op) {
                // 如果创建成功，利用多态执行计算
                op->forward();
            }
        }
        std::cout << "\n--- Engine Shutdown (RAII cleans up automatically) ---" << std::endl;
    }
}
