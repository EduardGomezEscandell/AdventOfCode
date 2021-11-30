cmake_minimum_required(VERSION 3.16)

project(${AOC_PROJECT_NAME} LANGUAGES C)
add_executable(${AOC_PROJECT_NAME} main.c)

add_subdirectory(../common common/${CMAKE_BUILD_TYPE})

add_compile_options(-Wall -Werror -Wextra -Wpedantic)

target_link_libraries(${AOC_PROJECT_NAME} mystring vector math)