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
