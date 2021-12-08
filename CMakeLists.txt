cmake_minimum_required(VERSION 3.16)

project(AOC_2021 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(common)

# Week 1
add_subdirectory(01)
add_subdirectory(02)
add_subdirectory(03)
add_subdirectory(04)
add_subdirectory(05)

# Week 2
add_subdirectory(06)
add_subdirectory(07)
add_subdirectory(08)

set(CMAKE_C_FLGAS -Wall -Werror -Wextra -Wpedantic)
