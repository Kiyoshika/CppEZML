project(CppEZML)
cmake_minimum_required(VERSION 3.1.0)

add_library(cppezml STATIC "")
add_subdirectory(src)

set(CMAKE_CXX_FLAGS "-O3")