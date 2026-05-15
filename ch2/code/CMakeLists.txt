# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(TensorOpsDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 将 main.cpp 和 tensor_ops.cpp 一起编译为可执行文件
add_executable(tensor_app main.cpp tensor_ops.cpp)