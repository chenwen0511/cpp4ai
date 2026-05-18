在Python的世界里，垃圾回收器（Garbage Collector）像一位尽职的清洁工，默默在后台清理不再使用的对象。而在C++中，你将接管控制权，直接向操作系统申请和释放内存。这种权力带来了极致的性能，也带来了极大的责任。

在处理底层优化和硬件部署时，熟练掌握动态内存管理是避免系统崩溃、保证程序7x24小时稳定运行的基石。

---

### 第一部分：理论基础 (45 分钟)

#### 1. 使用 `new` 和 `delete` 进行堆内存管理

上一课我们提到，栈（Stack）内存极小，无法容纳大型数据。当我们需要为大模型加载权重、或在运行时动态分配大量内存时，必须借助于堆（Heap）。

* **分配内存 (`new`):** 向操作系统申请一块指定大小的内存，并返回该内存的起始地址（指针）。
* **释放内存 (`delete`):** 告诉操作系统这块内存用完了，可以回收。

```cpp
// 1. 单个对象的分配与释放
int* p1 = new int;      // 分配一个 int 的空间
*p1 = 42;
delete p1;              // 释放空间

// 2. 数组的分配与释放（注意 delete[] 语法）
float* tensor_data = new float[10000]; // 分配 10000 个 float
delete[] tensor_data;                  // 释放数组必须加 []

```

#### 2. 内存泄漏 (Memory Leak) 与悬空指针 (Dangling Pointer)

手动管理内存最容易犯的两个致命错误：

* **内存泄漏：** 使用 `new` 申请了内存，但忘记调用 `delete`。
* **后果：** 程序在长时间运行（例如持续的巡检作业）中，消耗的内存会不断增加，最终导致系统OOM（Out Of Memory）崩溃。


* **悬空指针：** 内存已经被 `delete` 释放，但程序依然拿着旧的指针去读取或写入该地址。
* **后果：** 产生不可预知的行为，通常会直接导致 Segmentation Fault（段错误）。



#### 3. 核心哲学：RAII (Resource Acquisition Is Initialization)

这是C++管理资源最核心的理念，也是现代C++（如智能指针）的底层逻辑。
既然人类总是会忘记写 `delete`，那为什么不把“释放堆内存”的工作，交给生命周期由编译器自动管理的“栈对象”呢？

* **机制：** 将堆上申请的资源（如内存、文件句柄、硬件锁）封装在一个类的内部。
* **实现：** 在类的**构造函数**中申请资源（Acquisition），在类的**析构函数**中释放资源。
* **优势：** 当这个类对象在栈上的作用域结束时，编译器会自动调用其析构函数，从而百分之百确保内部封装的堆资源被安全释放，彻底杜绝内存泄漏。

---

### 第二部分：实操实验 (75 分钟)

#### 实验 1：在堆上分配一维数组模拟二维矩阵

在进行视觉巡检或处理深度学习特征图时，我们经常遇到多维张量。但在C++底层，为了保证内存连续性（提高CPU Cache命中率，以及方便在Host与Device间进行内存拷贝），我们**绝对不会**使用指针的指针（如 `float`）来构建二维数组，而是**在堆上分配一个巨大的一维数组，通过数学索引来模拟多维结构**。

**行主序 (Row-major) 索引公式：** 对于一个 `rows` 行 `cols` 列的矩阵，访问第 $i$ 行第 $j$ 列的元素，其一维索引为 $i \times cols + j$。

**1. 编写代码 (`matrix_demo.cpp`)**

```cpp
#include <iostream>
#include <iomanip> // 用于对齐输出

int main() {
    // 假设我们要为巡检系统的摄像头分配一帧图像或特征图的内存
    const int rows = 5;
    const int cols = 5;

    // 1. 在堆上动态分配连续的一维数组
    std::cout << "Allocating memory on the heap..." << std::endl;
    float* feature_map = new float[rows * cols];

    // 2. 模拟二维访问，进行赋值
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int index = i * cols + j; // 计算一维扁平化索引
            // 赋一个简单的测试值
            feature_map[index] = static_cast<float>(i + j); 
        }
    }

    // 3. 打印矩阵验证
    std::cout << "Feature Map Content:" << std::endl;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int index = i * cols + j;
            std::cout << std::setw(4) << feature_map[index] << " ";
        }
        std::cout << std::endl;
    }

    // 4. 正确释放内存 (关键步骤)
    delete[] feature_map;
    // 良好习惯：释放后将指针置空，防止产生悬空指针
    feature_map = nullptr; 

    std::cout << "Memory successfully freed." << std::endl;
    return 0;
}

```

#### 实验 2：使用 AddressSanitizer (ASan) 捕捉内存泄漏

在复杂的工程中，肉眼排查内存泄漏极其困难。在工业界（包括模型推理框架的开发中），开发者通常使用编译器自带的内存检测神器：**AddressSanitizer**（比传统的 Valgrind 速度快得多）。

**1. 制造内存泄漏**
打开刚才的 `matrix_demo.cpp`，将最后释放内存的代码注释掉：

```cpp
    // 故意注释掉，制造内存泄漏
    // delete[] feature_map; 
    // feature_map = nullptr; 

```

**2. 使用 ASan 标志编译代码**
在编译时，我们需要加入 `-fsanitize=address` 和 `-g`（生成调试信息，以便报错时显示行号）。

```bash
g++ -g -fsanitize=address matrix_demo.cpp -o matrix_demo_asan

```

**3. 运行程序并观察报错**
执行程序：

```bash
./matrix_demo_asan

```

输出
```
Allocating memory on the heap...
Feature Map Content:
   0    1    2    3    4 
   1    2    3    4    5 
   2    3    4    5    6 
   3    4    5    6    7 
   4    5    6    7    8 
Memory successfully freed.

=================================================================
==2957688==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 100 byte(s) in 1 object(s) allocated from:
    #0 0x725e7ceb6357 in operator new[](unsigned long) ../../../../src/libsanitizer/asan/asan_new_delete.cpp:102
    #1 0x62310a208357 in main /home/ubuntu/stephen/01-code/cpp4ai/ch4/code/matrix_demo.cpp:11
    #2 0x725e7c629d8f in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
```

**预期输出：**
程序会正常打印出矩阵内容，但在退出时，AddressSanitizer 会拦截并输出一段醒目的错误报告（通常带有红色字体的 `ERROR: LeakSanitizer: detected memory leaks`）。

报告的核心内容会精准指出：

1. **泄漏了多少字节：** 比如 `Direct leak of 100 byte(s)`（因为 25 个 float，每个 4 字节）。
2. **在哪里分配的：** 会明确指出是哪一个函数、哪一行代码使用了 `new`。

**课后修复：**
解除代码中的注释，重新编译并运行，确认错误报告消失。这种通过底层工具直接排查内存状态的技能，将极大提升代码在生产环境下的鲁棒性。