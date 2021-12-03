cmake_minimum_required(VERSION 3.16)

project(AOC_2021 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(common)

add_subdirectory(01)
add_subdirectory(02)
add_subdirectory(03)
add_subdirectory(04)

set(CMAKE_C_FLGAS -Wall -Werror -Wextra -Wpedantic)
