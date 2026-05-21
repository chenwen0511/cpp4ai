cmake_minimum_required(VERSION 3.12)
project(FastOpsProject)

set(CMAKE_CXX_STANDARD 17)

# 1. 查找 Python 环境
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# 2. 查找我们刚才通过 pip 安装的 pybind11
execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m pybind11 --includes
    OUTPUT_VARIABLE PYBIND11_INCLUDES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# 将获取到的路径转换为 CMake 能够识别的格式
separate_arguments(PYBIND11_INCLUDES UNIX_COMMAND "${PYBIND11_INCLUDES}")
# 移除 "-I" 前缀以便 CMake 使用
string(REPLACE "-I" "" PYBIND11_INCLUDES "${PYBIND11_INCLUDES}")

find_package(pybind11 CONFIG REQUIRED PATHS "${PYBIND11_INCLUDES}")

# 3. 使用 pybind11 专属指令编译模块 (注意：模块名 fast_ops 必须与 cpp 中的一致)
pybind11_add_module(fast_ops fast_ops.cpp)