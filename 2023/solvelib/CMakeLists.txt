add_library(solvelib
    01/day01.cpp
    02/day02.cpp    02/game.cpp
    03/day03.cpp
    04/day04.cpp
    05/day05.cpp

    11/day11.cpp
    12/day12.cpp
    13/day13.cpp
    14/day14.cpp
    15/day15.cpp
    16/day16.cpp
)

set_target_properties(solvelib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(solvelib INTERFACE ..)

target_link_libraries(solvelib PUBLIC xmaslib)

if (BUILD_TESTS)
    set(ENABLE_DOCTESTS 1)
    target_link_libraries(solvelib PUBLIC doctest)
endif()
