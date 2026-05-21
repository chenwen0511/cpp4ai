#include <iostream>
#include <vector>
#include <numeric>
#include <future>  // 包含 async 和 future
#include <chrono>

// 定义一个处理数据块的函数：计算 [start, end) 范围内的元素总和
// 注意：只读访问原数组，不会产生数据竞争，因此不需要 mutex！
double process_chunk(const std::vector<float>& data, size_t start, size_t end) {
    double sum = 0.0;
    for (size_t i = start; i < end; ++i) {
        sum += data[i];
    }
    return sum;
}

int main() {
    std::cout << "--- Parallel Preprocessing Pipeline ---" << std::endl;

    // 1. 准备海量数据 (模拟一亿个元素的特征向量，约 400MB)
    const size_t DATA_SIZE = 100'000'000;
    std::cout << "Initializing " << DATA_SIZE << " elements..." << std::endl;
    std::vector<float> features(DATA_SIZE, 1.5f); // 全部初始化为 1.5

    // 设置我们想要使用的并发线程数 (通常根据 CPU 核心数决定)
    const int NUM_THREADS = 4;
    const size_t CHUNK_SIZE = DATA_SIZE / NUM_THREADS;

    // --- 单线程基准测试 ---
    auto start_single = std::chrono::high_resolution_clock::now();
    double single_sum = process_chunk(features, 0, DATA_SIZE);
    auto end_single = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> time_single = end_single - start_single;
    std::cout << "[Single Thread] Sum: " << single_sum 
              << " | Time: " << time_single.count() << " ms" << std::endl;

    // --- 多线程并行测试 ---
    auto start_multi = std::chrono::high_resolution_clock::now();

    // 2. 启动并发任务
    // 使用 std::vector 保存所有后台任务返回的凭据 (futures)
    std::vector<std::future<double>> futures;

    for (int i = 0; i < NUM_THREADS; ++i) {
        size_t start_idx = i * CHUNK_SIZE;
        // 最后一个线程处理掉所有剩余的元素，防止除不尽
        size_t end_idx = (i == NUM_THREADS - 1) ? DATA_SIZE : start_idx + CHUNK_SIZE;

        // 使用 std::async 启动后台计算
        // std::launch::async 标志强制它必须启动一个新线程
        futures.push_back(
            std::async(std::launch::async, process_chunk, std::cref(features), start_idx, end_idx)
        );
    }

    // 3. 收集并聚合结果
    double multi_sum = 0.0;
    for (int i = 0; i < NUM_THREADS; ++i) {
        // future.get() 会阻塞当前主线程，直到对应的后台任务执行完毕
        multi_sum += futures[i].get(); 
    }

    auto end_multi = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> time_multi = end_multi - start_multi;

    std::cout << "[Multi Thread ] Sum: " << multi_sum 
              << " | Time: " << time_multi.count() << " ms" << std::endl;

    // 4. 计算加速比
    std::cout << "\nSpeedup: " << time_single.count() / time_multi.count() << "x" << std::endl;

    return 0;
}