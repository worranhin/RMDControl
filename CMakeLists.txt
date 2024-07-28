cmake_minimum_required(VERSION 3.10) # 设置CMake的最低版本需求

project(RMDControl VERSION 0.1) # 定义项目名称

# 设置C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# 添加一个可执行文件
add_executable(MyExecutable main.c RMDControl.c)