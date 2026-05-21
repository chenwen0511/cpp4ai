cmake_minimum_required(VERSION 3.12)
project(InferenceEngine)

# 要求使用 C++14 或更高标准（为了使用 std::make_unique）
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add_library 指令替代了 add_executable
# SHARED 关键字指示 CMake 生成一个动态链接库 (.so / .dll)
add_library(engine SHARED inference_engine.cpp)

# 设置输出的高级编译优化标志 (模拟 Release 发布)
target_compile_options(engine PRIVATE -O3 -Wall -fPIC)
