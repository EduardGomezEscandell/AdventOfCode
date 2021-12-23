set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion")

add_executable(aoc_2021_23 main.c day23.c routing.c gamestate.c)

target_link_libraries(aoc_2021_23 file_utils timing pthread)

