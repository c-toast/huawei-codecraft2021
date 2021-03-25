cmake_minimum_required(VERSION 3.17)
project(huawei_codecraft)

set(CMAKE_CXX_STANDARD 11)

# 添加头文件
#include_directories(
#        ${CMAKE_CURRENT_SOURCE_DIR}/googletest/include
#)
# 添加下级目录，生成.a文件
#add_subdirectory( ./googletest)

# 添加连接库
#link_directories(
#        ${CMAKE_CURRENT_SOURCE_DIR}/googletest
#)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(${PROJECT_SOURCE_DIR}/src DIR_SRCS)

# 指定生成目标
add_executable(CodeCraft-2021 ${DIR_SRCS} src/strategy.cpp src/vmmigrater.cpp src/vmdeployer.cpp)

target_include_directories(
        CodeCraft-2021 PRIVATE ${PROJECT_SOURCE_DIR}/include
)

# 将目标文件与库文件进行链接
#target_link_libraries(huawei_codecraft gtest)


#utils_test
#add_executable(utils_test utils.cpp utils_test.cpp)
#target_link_libraries(utils_test gtest)