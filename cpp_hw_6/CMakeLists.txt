cmake_minimum_required(VERSION 3.10)

project(hw6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(hw6 hw6.cpp)
target_link_libraries(hw6 PRIVATE Threads::Threads)