cmake_minimum_required(VERSION 3.16)
project(CHIP-8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Зависимости ---
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# --- Ядро эмулятора ---
add_library(chip8_core src/chip8.cpp)
target_include_directories(chip8_core PUBLIC src)

# --- Приложение ---
add_executable(chip8 src/main.cpp
    src/gui.h)

if(MINGW)
    set_target_properties(chip8 PROPERTIES
       WIN32_EXECUTABLE FALSE
    )
    # target_compile_definitions(chip8 PRIVATE SDL_MAIN_HANDLED)
    target_include_directories(chip8 PRIVATE
        "D:/Libs/SDL2"
        "D:/Libs/SDL2_ttf"
    )
endif()

if(WIN32 AND MINGW)
    add_custom_command(TARGET chip8 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Users/kva/Documents/gcc-native/mingw64/bin/libgcc_s_seh-1.dll"
            "C:/Users/kva/Documents/gcc-native/mingw64/bin/libstdc++-6.dll"
            "C:/Users/kva/Documents/gcc-native/mingw64/bin/libwinpthread-1.dll"
            "$<TARGET_FILE_DIR:chip8>"
    )
endif()

target_link_libraries(chip8 PRIVATE
    chip8_core
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
)

# --- Копирование DLL рядом с exe (только Windows) ---
if(WIN32)
    add_custom_command(TARGET chip8 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/libs/SDL2.dll"
            "${CMAKE_SOURCE_DIR}/libs/SDL2_ttf.dll"
            "$<TARGET_FILE_DIR:chip8>"
        COMMENT "Copying SDL2 .dll to folder with executable"
    )
endif()

# --- Тесты ---
find_package(GTest)
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