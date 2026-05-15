这份14节课（总计约28小时）的C++学习计划专为有Python背景的AI算法工程师设计。

Python开发者转向C++时，最大的思维转变在于**从“解释型+自动垃圾回收”向“编译型+手动内存管理”的跨越**。这份计划将重点对比Python和C++的差异，并强化底层内存布局、现代C++特性（C++11/14/17），以及对算法落地至关重要的性能优化概念。

---

### 第一阶段：从解释到编译（基础与构建）

**第1课：环境搭建与C++编译模型**

* **理论 (45 mins):**
* C++代码的生命周期：预处理 (Preprocessing) -> 编译 (Compilation) -> 汇编 (Assembly) -> 链接 (Linking)。
* GCC/Clang编译器基础命令。
* 初识构建工具：CMake 与 `CMakeLists.txt`（工业界标配）。


* **实操 (75 mins):**
* 安装开发环境（Linux/WSL下的g++与CMake）。
* 编写第一个 "Hello World"。
* 使用纯命令行 `g++` 编译多文件项目，感受链接错误 (Linker Error)。
* 编写一个基础的 `CMakeLists.txt` 来构建同一个多文件项目。



**第2课：强类型系统与基础控制流**

* **理论 (45 mins):**
* C++基础数据类型及其内存大小（`int`, `float`, `double`, `size_t`）。
* 静态类型与Python动态类型的区别，`auto` 关键字的使用。
* 控制流（`if`, `for`, `while`）及基于范围的for循环 (Range-based for loop)。
* 函数声明与定义的分类（头文件 `.h`/`.hpp` 与源文件 `.cpp` 的作用）。


* **实操 (75 mins):**
* 实现一个简单的矩阵加法函数。
* 将函数声明提取到头文件，并在另一个源文件中调用，配置CMake进行编译。



---

### 第二阶段：核心基石（内存与指针）

**第3课：指针、引用与地址**

* **理论 (60 mins):**
* 内存地址的概念。什么是栈 (Stack)，什么是堆 (Heap)。
* 指针 (`*`) 与取地址 (`&`)。指针的算术运算。
* 引用 (`&`) 的概念，以及它与指针、与Python对象引用的本质区别。
* 按值传递 (Pass by value) vs 按引用传递 (Pass by reference) 的性能差异（特别是对于大型结构体或Tensor）。


* **实操 (60 mins):**
* 编写函数，分别通过指针和引用来交换两个变量的值。
* 实现一个函数，接收大型数组的引用并修改其内容，对比传值和传引用的耗时。



**第4课：动态内存与资源管理 (RAII)**

* **理论 (45 mins):**
* 使用 `new` 和 `delete` 进行堆内存分配与释放。
* 内存泄漏 (Memory Leak) 与悬空指针 (Dangling Pointer) 的产生原因。
* **核心思想：RAII (Resource Acquisition Is Initialization)** —— C++管理资源的哲学（利用对象生命周期管理内存、文件句柄、锁）。


* **实操 (75 mins):**
* 手动在堆上分配一个一维数组，模拟二维矩阵的存储（行主序），进行赋值和打印，最后正确释放内存。
* 使用Valgrind或AddressSanitizer工具检测自己代码中故意留下的内存泄漏。



---

### 第三阶段：面向对象编程 (OOP)

**第5课：类与对象基础**

* **理论 (45 mins):**
* `class` 与 `struct` 的区别。
* 访问控制修饰符 (`public`, `private`, `protected`)。
* 构造函数 (Constructor) 与析构函数 (Destructor) —— 结合RAII思想。
* 成员初始化列表 (Initializer List) 的性能优势。


* **实操 (75 mins):**
* 封装一个简单的 `Tensor` 类：在构造函数中分配内存，在析构函数中释放内存，实现简单的加法操作。



**第6课：类的进阶（拷贝与赋值）**

* **理论 (60 mins):**
* 深拷贝 (Deep Copy) vs 浅拷贝 (Shallow Copy)。
* Rule of Three：拷贝构造函数 (Copy Constructor)、拷贝赋值运算符 (Copy Assignment Operator) 与析构函数的联动。
* `const` 成员函数与 `const` 引用。


* **实操 (60 mins):**
* 完善第5课的 `Tensor` 类，实现自定义的拷贝构造和赋值重载，避免默认浅拷贝导致的多次释放 (Double Free) 报错。



**第7课：继承与多态**

* **理论 (45 mins):**
* 继承的语法与内存模型。
* 虚函数 (`virtual`)、重写 (`override`) 与动态多态 (Dynamic Polymorphism)。
* 纯虚函数与抽象类（类似于Python的 `abc.ABC`）。
* 虚析构函数的重要性。


* **实操 (75 mins):**
* 设计一个 `Layer` 抽象基类，包含纯虚函数 `forward()`。
* 派生出 `LinearLayer` 和 `ReLULayer`，使用基类指针数组统一管理这些层并依次调用 `forward()`。



---

### 第四阶段：现代C++ (C++11/14/17)

**第8课：智能指针（告别手动 delete）**

* **理论 (45 mins):**
* 为什么需要智能指针？
* 独占所有权：`std::unique_ptr`。
* 共享所有权（引用计数）：`std::shared_ptr`（原理类似于Python的内存管理）。
* 避免循环引用：`std::weak_ptr`。


* **实操 (75 mins):**
* 重构第7课的神经网络层级管理代码，将裸指针 (Raw Pointers) 替换为 `std::unique_ptr`，体会自动内存管理的便利。



**第9课：移动语义与Lambda表达式**

* **理论 (60 mins):**
* 左值 (lvalue) 与右值 (rvalue)。
* 右值引用 (`&&`) 与移动语义 (`std::move`)：如何窃取资源以避免昂贵的深拷贝（对大规模数据传递极其关键）。
* Rule of Five：增加移动构造函数与移动赋值运算符。
* Lambda表达式语法与捕获列表 (`[]`, `[=]`, `[&]`)。


* **实操 (60 mins):**
* 为之前的 `Tensor` 类添加移动构造函数，测试其在函数返回大型对象时的性能提升。
* 使用Lambda表达式编写自定义排序逻辑。



---

### 第五阶段：标准模板库 (STL) 与泛型

**第10课：STL容器与字符串**

* **理论 (45 mins):**
* 序列容器：`std::vector` (动态数组，最常用，对应Python List), `std::array`, `std::deque`。
* 关联容器：`std::map` (对应Dict), `std::unordered_map` (哈希表), `std::set`。
* 现代C++字符串：`std::string` 及其底层机制。


* **实操 (75 mins):**
* 使用 `std::unordered_map` 实现一个简单的词表映射 (Token-to-ID Mapping) 程序，对比读取速度。



**第11课：STL算法、迭代器与模板编程**

* **理论 (60 mins):**
* 迭代器 (Iterator) 模式：连接容器与算法的桥梁。
* 常用STL算法 (`<algorithm>`)：`std::sort`, `std::transform`, `std::accumulate`。
* 泛型编程基础：函数模板 (Function Template) 与类模板 (Class Template)。


* **实操 (60 mins):**
* 编写一个模板函数，可以对 `std::vector<int>` 和 `std::vector<float>` 计算均值和方差。
* 结合STL算法和Lambda表达式，对一个包含自定义结构体的数据集进行过滤和排序。



---

### 第六阶段：高级特性与AI工程融合

**第12课：多线程与并发控制**

* **理论 (45 mins):**
* C++多线程基础：`std::thread`（突破Python GIL的限制，实现真正的并行）。
* 数据竞争 (Data Race) 与同步机制：`std::mutex`, `std::lock_guard`。
* 并发任务：`std::async` 与 `std::future`。


* **实操 (75 mins):**
* 实现一个多线程图像/数据预处理管道：将数据切分到多个线程中并行处理（例如并行对多个矩阵求和），并使用 `std::async` 收集结果。



**第13课：C++与Python混合编程 (Pybind11)**

* **理论 (45 mins):**
* 为什么需要混合编程？（Python作为胶水层负责调度，C++负责高性能计算）。
* Pybind11 库介绍与CMake集成。
* 数据类型在C++与Python之间的映射（特别是 `numpy` array 与 C++ 指针的交互）。


* **实操 (75 mins):**
* 使用 Pybind11 将你之前编写的高性能C++函数（例如多线程矩阵操作）打包成 Python 模块 `.so`。
* 在 Python 脚本中 `import` 该模块，传入 Numpy 数组并接收计算结果。



**第14课：综合项目——简易推理引擎设计**

* **理论与设计 (30 mins):**
* 回顾与梳理：如何结合面向对象、RAII、多线程和智能指针构建健壮的系统架构。


* **实操 (90 mins):**
* 实现一个极简版的“算子注册表”：使用工厂模式 (Factory Pattern) 和 `std::map`，通过字符串名称动态创建对应的算子实例 (如 "Conv2D", "ReLU")。
* 编写CMake将其编译为动态链接库 (Shared Library)。