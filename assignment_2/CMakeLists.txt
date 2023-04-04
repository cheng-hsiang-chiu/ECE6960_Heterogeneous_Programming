# CMake version
cmake_minimum_required (VERSION 3.12)

# Project name
project(fp VERSION 1.0 LANGUAGES CXX)

include (cmake/benchmark.cmake)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++17 -O3 -pthread")
# set(FM_3RD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3rd-party)

add_library(error_settings INTERFACE)

add_executable(main ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_include_directories(main PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

include_directories(${CMAKE_BINARY_DIR}/benchmark/build/include)

target_link_libraries(main gbenchmark)

