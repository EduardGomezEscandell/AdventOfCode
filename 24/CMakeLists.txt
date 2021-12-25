add_executable(aoc_2021_24 main.c day24.c virtual_machine.c simulate.c)

target_link_libraries(aoc_2021_24 file_utils timing m)

# set(CMAKE_C_FLAGS "-Wconversion")