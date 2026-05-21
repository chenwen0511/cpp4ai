#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>

int main() {
    std::cout << "--- Token-to-ID Vocabulary Benchmark ---" << std::endl;

    const int VOCAB_SIZE = 100000;   // 模拟 10 万个词表的规模
    const int QUERY_COUNT = 50000;   // 模拟 5 万次高频查询

    std::map<std::string, int> ordered_vocab;
    std::unordered_map<std::string, int> unordered_vocab;
    std::vector<std::string> query_tokens;

    // 1. 构建测试词表数据
    std::cout << "Building vocabulary with " << VOCAB_SIZE << " tokens..." << std::endl;
    for (int i = 0; i < VOCAB_SIZE; ++i) {
        std::string token = "token_" + std::to_string(i);
        ordered_vocab[token] = i;
        unordered_vocab[token] = i;
        
        // 收集一部分 token 用于后续的查询测试
        if (i % 2 == 0 && query_tokens.size() < QUERY_COUNT) {
            query_tokens.push_back(token);
        }
    }

    // 2. 测试 std::map (红黑树 O(log N)) 的查询性能
    long long map_checksum = 0; // 防止编译器把没用到结果的查询循环优化掉
    auto start_map = std::chrono::high_resolution_clock::now();
    
    for (const auto& token : query_tokens) {
        map_checksum += ordered_vocab[token];
    }
    
    auto end_map = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> map_time = end_map - start_map;

    // 3. 测试 std::unordered_map (哈希表 O(1)) 的查询性能
    long long umap_checksum = 0;
    auto start_umap = std::chrono::high_resolution_clock::now();
    
    for (const auto& token : query_tokens) {
        umap_checksum += unordered_vocab[token];
    }
    
    auto end_umap = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> umap_time = end_umap - start_umap;

    // 4. 打印结果
    std::cout << "\n[std::map] Time for " << QUERY_COUNT << " queries: " 
              << map_time.count() << " ms" << std::endl;
    std::cout << "[std::unordered_map] Time for " << QUERY_COUNT << " queries: " 
              << umap_time.count() << " ms" << std::endl;

    // 确保校验和一致，证明查询结果完全正确
    if (map_checksum == umap_checksum) {
        std::cout << "\nChecksum verified successfully." << std::endl;
    }

    return 0;
}