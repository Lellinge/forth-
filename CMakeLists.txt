cmake_minimum_required(VERSION 3.27)
project(simpleforth)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

add_executable(simpleforth main.cpp)


