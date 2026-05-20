#include <iostream>
#include <vector>
#include <algorithm> // 包含 std::sort

// 定义一个简单的边界框结构体
struct BoundingBox {
    int id;
    float confidence;
    float area;
};

int main() {
    std::vector<BoundingBox> bboxes = {
        {1, 0.85f, 1200.5f},
        {2, 0.99f, 800.0f},
        {3, 0.45f, 3500.2f},
        {4, 0.92f, 150.0f}
    };

    std::cout << "--- 原始顺序 ---" << std::endl;
    for (const auto& box : bboxes) {
        std::cout << "ID: " << box.id << " | Conf: " << box.confidence << std::endl;
    }

    // 需求 1：按置信度 (confidence) 降序排列
    // 使用 Lambda 表达式作为 std::sort 的自定义比较器
    std::sort(bboxes.begin(), bboxes.end(), 
        [](const BoundingBox& a, const BoundingBox& b) {
            return a.confidence > b.confidence; // 返回 true 表示 a 应该排在 b 前面
        }
    );

    std::cout << "\n--- 按置信度降序排列 ---" << std::endl;
    for (const auto& box : bboxes) {
        std::cout << "ID: " << box.id << " | Conf: " << box.confidence << std::endl;
    }

    // 需求 2：使用捕获列表过滤数据
    float conf_threshold = 0.90f;
    int pass_count = 0;

    // [&pass_count, conf_threshold]：按引用捕获 pass_count 以便修改，按值捕获阈值
    std::cout << "\n--- 过滤高置信度框 ---" << std::endl;
    for (const auto& box : bboxes) {
        // 定义一个就地执行的 Lambda 进行检查
        auto check_pass = [&pass_count, conf_threshold](float conf) {
            if (conf >= conf_threshold) {
                pass_count++; // 直接修改外部变量
                return true;
            }
            return false;
        };

        if (check_pass(box.confidence)) {
            std::cout << "ID " << box.id << " passed." << std::endl;
        }
    }

    std::cout << "Total boxes passed threshold: " << pass_count << std::endl;

    return 0;
}