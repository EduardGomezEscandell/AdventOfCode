add_library(solvelib
    alldays.hpp
    01/day01.hpp    01/day01.cpp 01/day01_test.hpp
    02/day02.hpp    02/day02.cpp 02/day02_test.hpp 02/game.hpp    02/game.cpp
    03/day03.hpp    03/day03.cpp 03/day03_test.hpp
    04/day04.hpp    04/day04.cpp 04/day04_test.hpp
    05/day05.hpp    05/day05.cpp 05/day05_test.hpp
)

set_target_properties(solvelib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(solvelib INTERFACE ..)

target_link_libraries(solvelib PUBLIC xmaslib)

if (BUILD_TESTS)
    set(ENABLE_DOCTESTS 1)
    target_link_libraries(solvelib PUBLIC doctest)
endif()
