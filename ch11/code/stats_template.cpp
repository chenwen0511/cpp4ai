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