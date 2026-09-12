cmake_minimum_required(VERSION 3.10)
project(CHIP-8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

find_library(GLU_LIBRARY NAMES GLU PATHS /usr/lib64 /usr/lib REQUIRED)

add_executable(chip8 
    src/main.cpp
    src/chip8.h
    src/chip8.cpp)

target_link_libraries(chip8 PRIVATE OpenGL::GL GLUT::GLUT ${GLU_LIBRARY})