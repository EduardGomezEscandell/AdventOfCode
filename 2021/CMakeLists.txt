cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra -Wpedantic")

project(AOC_2021 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(common)

if(ADRESS_SANITIZER)
    message("Adress sanitizer enabled")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
endif()

if(THREAD_SANITIZER)
    message("Adress sanitizer enabled")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread -fsanitize=undefined")
endif()

message("-- C compiler flags: ${CMAKE_C_FLAGS}")

if(INCLUDE_SAMPLES)
    message("-- Samples included")
    add_subdirectory(sample)
endif()


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
add_subdirectory(15)
add_subdirectory(16)
add_subdirectory(17)
add_subdirectory(18)
add_subdirectory(19)

# Week 3
add_subdirectory(20)
add_subdirectory(21)
add_subdirectory(22)
add_subdirectory(23)
add_subdirectory(24)
add_subdirectory(25)
