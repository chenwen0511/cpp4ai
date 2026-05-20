欢迎来到第9课。这是C++11引入的两次“大地震”级别的新特性：**移动语义 (Move Semantics)** 和 **Lambda 表达式**。

在AI工程中，当你需要在不同的处理模块（例如：预处理流水线 -> 推理引擎 -> 后处理）之间传递几MB到几十MB的特征图或图像帧时，深拷贝带来的时间损耗是不可接受的。移动语义允许我们以 $O(1)$ 的时间复杂度“窃取”底层内存块的所有权，实现真正的**零拷贝 (Zero-copy)**。而Lambda表达式则为C++带来了现代的函数式编程体验。

---

### 第一部分：理论基础 (60 分钟)

#### 1. 左值 (lvalue) 与右值 (rvalue)

在理解“移动”之前，必须先分清数据的生死状态。

* **左值 (lvalue):** 可以放在等号左边的值。它在内存中有明确的地址，有持久的生命周期（具名变量）。
* *特征：* 可以对它取地址 `&a`。


* **右值 (rvalue):** 只能放在等号右边的值。通常是临时的、没有名字的、即将消亡的值（如字面量、函数返回的临时对象）。
* *特征：* 无法取地址。一旦当前语句执行完毕，它就会灰飞烟灭。



```cpp
int a = 10;      // 'a' 是左值，'10' 是右值
int b = a + 5;   // 'b' 是左值，'a + 5' 产生的临时结果是右值

```

#### 2. 右值引用 (`&&`) 与 `std::move`

* **右值引用 (`&&`):** 专门用来绑定到右值上的引用。当你使用 `&&` 接收一个对象时，你是在告诉编译器：“这个对象马上就要死了，它肚子里的资源（如分配好的堆内存）我可以放心大胆地**洗劫一空**，而不用担心影响别人。”
* **`std::move` 的本质：** 这是一个类型转换函数。它**不移动任何东西**，它的唯一作用是：把一个左值强制转换为右值引用 (`&&`)，相当于你给编译器发了一份免责声明：“我保证以后不再使用这个变量了，你可以把它的资源偷走。”

#### 3. Rule of Five（五法则）

在第6课的 Rule of Three（析构、拷贝构造、拷贝赋值）基础上，为了支持移动语义，现代C++类需要补全另外两个函数：

1. **移动构造函数 (Move Constructor):** `Tensor(Tensor&& other)`
* *逻辑：* 直接把 `other` 的指针据为己有，然后把 `other` 的指针置为 `nullptr`。时间复杂度 $O(1)$。


2. **移动赋值运算符 (Move Assignment Operator):** `Tensor& operator=(Tensor&& other)`
* *逻辑：* 先释放自己原有的资源，再窃取 `other` 的资源，最后把 `other` 置空。



#### 4. Lambda表达式与捕获列表

如果你熟悉Python的 `lambda x: x * 2`，C++的Lambda理念相同，但能力强大得多。它允许你在函数内部定义匿名函数，这在使用STL算法（如排序、过滤）时极为方便。

**基本语法：** `[捕获列表](参数列表) -> 返回值类型 { 函数体 }`

* **捕获列表 (Capture List)：** 决定了Lambda内部可以访问外部作用域的哪些变量。
* `[]`：不捕获任何外部变量。
* `[=]`：按**值**捕获所有外部变量（内部得到的是副本，只读）。
* `[&]`：按**引用**捕获所有外部变量（可以修改外部变量，且无拷贝开销，极度常用）。
* `[x, &y]`：按值捕获 `x`，按引用捕获 `y`。



---

### 第二部分：实操实验 (60 分钟)

#### 实验 1：为 Tensor 类添加移动语义并测试性能

我们将演示当一个函数返回一个大型 `Tensor` 对象时，深拷贝与移动语义的巨大差异。

**1. 编写代码 (`move_semantics.cpp`)**

```cpp
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
    Tensor(const Tensor&) = delete; // 实验中我们可以暂时删除或保留拷贝构造，这里用代码逻辑演示
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

```

*注意：在现代C++编译器中，即使你不写移动构造，编译器也会尝试一种叫 RVO (Return Value Optimization) 的优化。但显式实现 Rule of Five 依然是管理底层内存的标准范式，尤其在使用 `std::move` 主动转移对象所有权时（如将数据移交入多线程队列）。*

#### 实验 2：使用 Lambda 表达式实现自定义排序

在目标检测（Object Detection）算法的 NMS（非极大值抑制）阶段，我们经常需要根据置信度对检测框进行排序。我们用 Lambda 来实现这段逻辑。

**1. 编写代码 (`lambda_sort.cpp`)**

```cpp
#include <iostream>
#include <vector>
#include <algorithm> // 包含 std::sort

// 定义一个简单的边界框结构体
struct BoundingBox {
    int id;
    float confidence;
    float area;
};

int main() {
    std::vector<BoundingBox> bboxes = {
        {1, 0.85f, 1200.5f},
        {2, 0.99f, 800.0f},
        {3, 0.45f, 3500.2f},
        {4, 0.92f, 150.0f}
    };

    std::cout << "--- 原始顺序 ---" << std::endl;
    for (const auto& box : bboxes) {
        std::cout << "ID: " << box.id << " | Conf: " << box.confidence << std::endl;
    }

    // 需求 1：按置信度 (confidence) 降序排列
    // 使用 Lambda 表达式作为 std::sort 的自定义比较器
    std::sort(bboxes.begin(), bboxes.end(), 
        [](const BoundingBox& a, const BoundingBox& b) {
            return a.confidence > b.confidence; // 返回 true 表示 a 应该排在 b 前面
        }
    );

    std::cout << "\n--- 按置信度降序排列 ---" << std::endl;
    for (const auto& box : bboxes) {
        std::cout << "ID: " << box.id << " | Conf: " << box.confidence << std::endl;
    }

    // 需求 2：使用捕获列表过滤数据
    float conf_threshold = 0.90f;
    int pass_count = 0;

    // [&pass_count, conf_threshold]：按引用捕获 pass_count 以便修改，按值捕获阈值
    std::cout << "\n--- 过滤高置信度框 ---" << std::endl;
    for (const auto& box : bboxes) {
        // 定义一个就地执行的 Lambda 进行检查
        auto check_pass = [&pass_count, conf_threshold](float conf) {
            if (conf >= conf_threshold) {
                pass_count++; // 直接修改外部变量
                return true;
            }
            return false;
        };

        if (check_pass(box.confidence)) {
            std::cout << "ID " << box.id << " passed." << std::endl;
        }
    }

    std::cout << "Total boxes passed threshold: " << pass_count << std::endl;

    return 0;
}

```
```
(base) ubuntu@ubuntu-System-Product-Name:~/stephen/01-code/cpp4ai/ch9/code$ ./lambda_sort 
--- 原始顺序 ---
ID: 1 | Conf: 0.85
ID: 2 | Conf: 0.99
ID: 3 | Conf: 0.45
ID: 4 | Conf: 0.92

--- 按置信度降序排列 ---
ID: 2 | Conf: 0.99
ID: 4 | Conf: 0.92
ID: 1 | Conf: 0.85
ID: 3 | Conf: 0.45

--- 过滤高置信度框 ---
ID 2 passed.
ID 4 passed.
Total boxes passed threshold: 2
```

编译并运行上述代码，你将直观地感受到 Lambda 表达式在处理数据集合时的灵活性。这种写法避免了在类外定义大量的单次使用的比较函数，极大提升了代码的内聚性和可读性。