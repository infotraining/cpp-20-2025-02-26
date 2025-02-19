cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_MODULE_STD 1)
project(import-std-lib)

# C++ needs to be enabled.
set(CMAKE_CXX_STANDARD 23)

add_executable(import-std-lib main.cpp)