cmake_minimum_required(VERSION 3.10)

# 1. Define the project name
project(MyProject)

# 2. Define the executable name and source files
# This replaces: g++ main.cpp -o main
add_executable(hello hello.cpp)