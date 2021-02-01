cmake_minimum_required(VERSION 3.17)
project(istrstr)

set(CMAKE_CXX_STANDARD 20)

add_executable(istrstr src/benchmark.cpp src/istrstr.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")

find_package(fmt)
target_link_libraries(istrstr fmt::fmt)
