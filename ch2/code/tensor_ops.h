// tensor_ops.h
#pragma once

#include <cstddef>

// 函数声明：将 array_a 和 array_b 对应位置的元素相加，结果存入 array_out
// 参数 length 使用 size_t，确保可以处理非常大的数组
void addArrays(const float* array_a, const float* array_b, float* array_out, size_t length);
