cmake_minimum_required(VERSION 3.16)
project(CHIP-8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Зависимости ---
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_library(GLU_LIBRARY NAMES GLU PATHS /usr/lib64 /usr/lib REQUIRED)

# --- Ядро эмулятора ---
add_library(chip8_core src/chip8.cpp)
target_include_directories(chip8_core PUBLIC src)

# --- Приложение ---
add_executable(chip8 src/main.cpp)
target_link_libraries(chip8 PRIVATE
    chip8_core
    OpenGL::GL
    GLUT::GLUT
    ${GLU_LIBRARY})

# --- Тесты (опционально, если GTest найден) ---
find_package(GTest QUIET)
if(GTest_FOUND)
    enable_testing()
    add_executable(chip8_tests tests/chip8_test.cpp)
    target_link_libraries(chip8_tests PRIVATE chip8_core GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(chip8_tests)
    message(STATUS "GoogleTest found — tests enabled")
else()
    message(STATUS "GoogleTest not found — tests disabled")
endif()