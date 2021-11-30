cmake_minimum_required(VERSION 3.16)

project($PROJECT_NAME LANGUAGES C)
add_executable(${AOC_PROJECT_NAME} main.c)

add_compile_options(-Wall -Werror -Wextra -Wpedantic)