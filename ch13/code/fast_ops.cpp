#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

// 核心算法：接收 numpy 数组并就地将所有元素翻倍
void scale_tensor_inplace(py::array_t<float> input_array) {
    // 1. 获取底层缓冲区的控制权
    py::buffer_info buf = input_array.request();

    // 2. 获取裸指针 (由于是 numpy 数组，底层必定是连续内存)
    float* ptr = static_cast<float*>(buf.ptr);
    size_t size = buf.size;

    std::cout << "[C++] Received array of size: " << size << std::endl;

    // 3. 直接在原内存上执行高速计算
    for (size_t i = 0; i < size; ++i) {
        ptr[i] *= 2.0f;
    }
}

// Pybind11 绑定宏 (模块名必须与生成 .so 文件的名称严格一致)
PYBIND11_MODULE(fast_ops, m) {
    m.doc() = "High-performance C++ backend for fast tensor operations"; // 模块文档字符串
    
    // 注册函数
    m.def("scale_tensor", &scale_tensor_inplace, "Multiply a NumPy array by 2 in-place");
}