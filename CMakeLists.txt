cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra -Wpedantic")

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
add_subdirectory(09)
add_subdirectory(10)
add_subdirectory(11)
add_subdirectory(12)

# Week 3
add_subdirectory(13)
add_subdirectory(14)