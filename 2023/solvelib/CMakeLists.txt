add_library(solvelib
    01/day01.cpp
    02/day02.cpp    02/game.cpp
    03/day03.cpp
    04/day04.cpp
    05/day05.cpp
    06/day06.cpp
    07/day07.cpp
    08/day08.cpp
    09/day09.cpp
    10/day10.cpp
    11/day11.cpp
    12/day12.cpp
    13/day13.cpp
    14/day14.cpp
    15/day15.cpp
    16/day16.cpp
    17/day17.cpp
    18/day18.cpp
    19/day19.cpp
    20/day20.cpp
    21/day21.cpp
    22/day22.cpp
    23/day23.cpp
    24/day24.cpp
)

set_target_properties(solvelib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(solvelib INTERFACE ..)

target_link_libraries(solvelib PUBLIC xmaslib)

if (BUILD_TESTS)
    set(ENABLE_DOCTESTS 1)
    target_link_libraries(solvelib PUBLIC doctest)
endif()
