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

add_executable(huawei_codecraft src/main.cpp src/filereadwriter.cpp)

target_include_directories(
        huawei_codecraft PRIVATE ${PROJECT_SOURCE_DIR}/include
)

# 将目标文件与库文件进行链接
#target_link_libraries(huawei_codecraft gtest)


#utils_test
#add_executable(utils_test utils.cpp utils_test.cpp)
#target_link_libraries(utils_test gtest)