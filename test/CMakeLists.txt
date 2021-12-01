cmake_minimum_required(VERSION 3.16)

project(aoc_2021_test LANGUAGES C)
add_executable(aoc_2021_test main.c)

add_compile_options(-Wall -Werror -Wextra -Wpedantic)

target_link_libraries(aoc_2021_test mystring vector math)