cmake_minimum_required(VERSION 3.29)

# Set experimental flag to enable `import std` support from CMake.
# This must be enabled before C++ language support.
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
# This specific value changes as experimental support evolves. See
# `Help/dev/experimental.rst` in the CMake source corresponding to
# your CMake build for the exact value to use.
"0e5b6991-d74f-4b3d-a41c-cf096e0b2508")

set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_FLAGS "-stdlib=libc++") # uncomment for Clang

project(cpp_modules_cmake)

add_subdirectory(modules-1)
add_subdirectory(modules-2)
add_subdirectory(modules-3)
# add_subdirectory(drawing-app)
# add_subdirectory(import-std/modules-4)
# add_subdirectory(import-std/drawing-app)
# add_subdirectory(import-std/import-std-lib)