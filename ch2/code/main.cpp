// main.cpp
#include <iostream>
#include "tensor_ops.h" // 包含我们的自定义头文件

int main() {
    // 模拟两个长度为 5 的一维特征向量 (FP32)
    const size_t feature_length = 5;
    float vec_a[feature_length] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    float vec_b[feature_length] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // 用于存放输出结果的数组
    float vec_out[feature_length] = {0};

    // 调用我们在 tensor_ops.cpp 中实现的函数
    addArrays(vec_a, vec_b, vec_out, feature_length);

    // 使用基于范围的for循环打印结果
    std::cout << "Output feature vector: ";
    for (auto val : vec_out) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    return 0;
}
