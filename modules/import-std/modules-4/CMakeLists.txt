cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_MODULE_STD 1)
project(primes)

# C++ needs to be enabled.
set(CMAKE_CXX_STANDARD 23)

add_library(math-with-import-std_lib)

target_sources(math-with-import-std_lib
  PUBLIC
    FILE_SET CXX_MODULES FILES
    math.cxx
    primes.cxx
    fibonacci_seq.cxx
)

add_executable(math-with-import-std math_main.cpp)
target_link_libraries(math-with-import-std PRIVATE math-with-import-std_lib)