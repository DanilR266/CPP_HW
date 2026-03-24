cmake_minimum_required(VERSION 3.14)
project(cpp_hw_2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(runTests test_cpp_hw_2.cpp)

target_link_libraries(runTests GTest::GTest GTest::Main pthread)

find_package(benchmark REQUIRED)

add_executable(runBenchmarks benchmark_cpp_hw_2.cpp)

target_link_libraries(runBenchmarks benchmark::benchmark pthread)