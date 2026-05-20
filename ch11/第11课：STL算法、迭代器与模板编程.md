欢迎来到第11课。在Python中，如果我们需要对一个列表求和，直接调用 `sum(lst)` 即可；如果要把所有元素乘以2，我们会用列表推导式 `[x * 2 for x in lst]`。这些极其简便的操作，在C++中是如何实现的呢？

这就是标准模板库（STL）中**算法、迭代器与模板**的三位一体。理解了它们，你就能用C++写出像Python一样优雅，且性能逼近底层硬件极限的代码。

---

### 第一部分：理论基础 (60 分钟)

#### 1. 迭代器 (Iterator) 模式：连接数据与算法的桥梁

在C++的设计哲学中，**数据容器（如 `vector`, `map`）和操作逻辑（如排序、查找）是严格解耦的**。那么，排序算法怎么知道它面对的是一个连续内存的数组，还是一个树形结构的字典呢？答案就是迭代器。

* **本质：** 迭代器是**指针的泛化**。它重载了 `++`（移到下一个元素）、`*`（解引用获取数据）等运算符。
* **半开区间 `[begin, end)`：** 所有的STL算法接收的都是一个左闭右开的范围。
* `vec.begin()`：指向第一个元素。
* `vec.end()`：指向**最后一个元素的下一个位置**（越界位置）。这种设计极其巧妙，当 `begin == end` 时，就代表容器为空。



#### 2. 常用STL算法 (`<algorithm>` 与 `<numeric>`)

C++标准库提供了上百个高度优化的通用算法，它们都可以接收任何容器的迭代器。

* **`std::sort`:** 使用Introsort（内省排序：快排 + 堆排 + 插入排序的混合体），平均时间复杂度 O(N log N)。
* **`std::transform`:** 完美对应Python的 `map()`。将一个函数（或Lambda）应用到区间内的每一个元素，并将结果写入目标位置。
* **`std::accumulate` (包含在 `<numeric>` 中):** 对应Python的 `sum()` 或 `functools.reduce()`。用于对区间内的元素进行累加（或自定义的累积操作）。

#### 3. 泛型编程基础：模板 (Templates)

Python是动态类型语言，你可以写 `def add(a, b): return a + b`，传入整数或浮点数都能运行。而在C++中，类型是静态绑定的，为了实现类似的功能，我们需要使用**模板**。

* **核心理念：** 告诉编译器一种“代码生成规则”，当编译器遇到具体的类型（如 `int` 或 `float`）时，**自动为你生成**对应类型的实体代码（这被称为模板的实例化）。
* **函数模板：**
```cpp
template <typename T>
T add(T a, T b) {
    return a + b;
}

```


当调用 `add(1.0f, 2.0f)` 时，编译器会在底层偷偷生成一份接收 `float` 的函数代码。
* **类模板：** 我们一直在用的 `std::vector<int>` 就是一个类模板，尖括号里的 `int` 就是传给模板的类型参数。

---

### 第二部分：实操实验 (60 分钟)

#### 实验 1：使用模板与算法计算均值和方差

在机器学习特征工程中，我们经常需要对数据进行标准化。我们将编写一个泛型函数，无论传入是 `vector<int>` 还是 `vector<float>`，都能计算其均值和方差。

均值计算公式：


$$ \mu = \frac{1}{N} \sum_{i=1}^{N} x_i $$

方差计算公式：


$$ \sigma^2 = \frac{1}{N} \sum_{i=1}^{N} (x_i - \mu)^2 $$

**1. 编写代码 (`stats_template.cpp`)**

```cpp
#include <iostream>
#include <vector>
#include <numeric>   // std::accumulate
#include <algorithm> // std::for_each
#include <cmath>

// 模板函数，返回一个包含均值和方差的 pair
template <typename T>
std::pair<double, double> calculate_stats(const std::vector<T>& data) {
    if (data.empty()) return {0.0, 0.0};

    // 1. 计算均值
    // std::accumulate 的第三个参数是初始值，它的类型决定了累加器的类型。
    // 这里使用 0.0 以确保累加过程使用双精度浮点数，防止大数组整数溢出。
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    double mean = sum / data.size();

    // 2. 计算方差
    // 使用 Lambda 表达式累加每个元素与均值差的平方
    double sq_diff_sum = 0.0;
    std::for_each(data.begin(), data.end(), [&sq_diff_sum, mean](T x) {
        double diff = static_cast<double>(x) - mean;
        sq_diff_sum += diff * diff;
    });
    
    double variance = sq_diff_sum / data.size();

    return {mean, variance};
}

int main() {
    std::vector<int> int_features = {1, 2, 3, 4, 5};
    std::vector<float> float_features = {1.5f, 2.5f, 3.5f, 4.5f, 5.5f};

    // 编译器会自动推导 T 为 int
    auto int_stats = calculate_stats(int_features);
    std::cout << "Int Features -> Mean: " << int_stats.first 
              << ", Variance: " << int_stats.second << std::endl;

    // 编译器会自动推导 T 为 float
    auto float_stats = calculate_stats(float_features);
    std::cout << "Float Features -> Mean: " << float_stats.first 
              << ", Variance: " << float_stats.second << std::endl;

    return 0;
}

```

#### 实验 2：结合算法与 Lambda 过滤和排序数据集

假设你的目标检测模型输出了几十个边界框结果，你需要先剔除置信度低于阈值的框，然后将剩下的框按面积从大到小排序。我们使用经典的**擦除-移除惯用法 (Erase-Remove Idiom)** 来实现就地过滤。

**1. 编写代码 (`filter_sort.cpp`)**

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

struct DetectionResult {
    int id;
    float confidence;
    float area;
};

int main() {
    std::vector<DetectionResult> results = {
        {1, 0.95f, 400.0f},
        {2, 0.40f, 1500.0f}, // 置信度低，待过滤
        {3, 0.88f, 1200.0f},
        {4, 0.20f, 300.0f},  // 置信度低，待过滤
        {5, 0.99f, 800.0f}
    };

    std::cout << "Original size: " << results.size() << std::endl;

    // 1. 过滤数据：移除置信度低于 0.5 的框
    // std::remove_if 并不会真正删除元素，而是把符合条件的元素移到末尾，
    // 并返回一个指向“新逻辑末尾”的迭代器。
    auto new_end = std::remove_if(results.begin(), results.end(), 
        [](const DetectionResult& res) {
            return res.confidence < 0.5f; // 返回 true 的将被移除
        }
    );
    
    // 必须调用 vector 自身的 erase 方法，才能真正从内存中削减容器的 size
    results.erase(new_end, results.end());
    
    std::cout << "Size after filtering: " << results.size() << std::endl;

    // 2. 排序数据：按面积降序排列
    std::sort(results.begin(), results.end(), 
        [](const DetectionResult& a, const DetectionResult& b) {
            return a.area > b.area;
        }
    );

    // 3. 打印结果
    std::cout << "--- Final Sorted Results ---" << std::endl;
    for (const auto& res : results) {
        std::cout << "ID: " << res.id 
                  << " | Conf: " << res.confidence 
                  << " | Area: " << res.area << std::endl;
    }

    return 0;
}

```

*预期输出提示：* 代码将首先把 `size` 从 5 缩减为 3，然后打印出 ID 分别为 3, 5, 1 的检测结果（面积递减）。在C++20中，`std::erase_if` 进一步简化了这个过程，但在工业界的 C++11/14 标准下，这种 `remove_if` + `erase` 的组合是极其核心的基本功。

```</DetectionResult></T>

```