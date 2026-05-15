# 1. 指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 2. 定义项目名称
project(RobotApp)

# 3. 指定C++标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 4. 添加可执行文件目标
# 语法：add_executable(可执行文件名 源文件1 源文件2 ...)
add_executable(my_app main.cpp robot_controller.cpp)