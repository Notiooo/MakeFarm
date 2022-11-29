cmake_minimum_required(VERSION 3.16)

enable_testing()
project(MakeFarm VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(tests)