#include <iostream>
#include <iomanip> // 用于对齐输出

int main() {
    // 假设我们要为巡检系统的摄像头分配一帧图像或特征图的内存
    const int rows = 5;
    const int cols = 5;

    // 1. 在堆上动态分配连续的一维数组
    std::cout << "Allocating memory on the heap..." << std::endl;
    float* feature_map = new float[rows * cols];

    // 2. 模拟二维访问，进行赋值
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int index = i * cols + j; // 计算一维扁平化索引
            // 赋一个简单的测试值
            feature_map[index] = static_cast<float>(i + j); 
        }
    }

    // 3. 打印矩阵验证
    std::cout << "Feature Map Content:" << std::endl;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int index = i * cols + j;
            std::cout << std::setw(4) << feature_map[index] << " ";
        }
        std::cout << std::endl;
    }

    // 4. 正确释放内存 (关键步骤)
    // delete[] feature_map;
    // 良好习惯：释放后将指针置空，防止产生悬空指针
    // feature_map = nullptr; 

    std::cout << "Memory successfully freed." << std::endl;
    return 0;
}