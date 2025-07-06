cmake_minimum_required(VERSION 3.31)
project(assignment5)

set(CMAKE_CXX_STANDARD 20)

add_executable(assignment5 main.cpp
        ast_nodes.h
        context.h
        context.cpp
        ast_nodes.cpp
        parser.h
        parser.cpp)
