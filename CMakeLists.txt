cmake_minimum_required(VERSION 3.10)
project(CHIP-8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chip8 src/main.cpp
    src/chip8.h)