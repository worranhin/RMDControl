cmake_minimum_required(VERSION 3.10) # 设置CMake的最低版本需求

project(RMDControl VERSION 0.1) # 定义项目名称

# 设置C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(RMDControl RMDControl.cpp)
add_library(RMDMotor RMDMotor.cpp)
add_library(SerialPort SerialPort.cpp)
target_link_libraries(RMDMotor SerialPort)

# 添加一个可执行文件
add_executable(RMDControl_main main.cpp)
target_link_libraries(RMDControl_main RMDControl)
target_link_libraries(RMDControl_main RMDMotor)
target_link_libraries(RMDControl_main SerialPort)

# add_executable(RMDControl_main main.c)
# target_link_libraries(RMDControl_main RMDControl)
# add_executable(RMDControl_test test.c)
# target_link_libraries(RMDControl_test RMDControl)

# add_library(RMD RMD.cpp)
