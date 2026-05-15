// tensor_ops.cpp
#include "tensor_ops.h"

void addArrays(const float* array_a, const float* array_b, float* array_out, size_t length) {
    // 使用传统的for循环遍历内存
    for (size_t i = 0; i < length; ++i) {
        array_out[i] = array_a[i] + array_b[i];
    }
}
