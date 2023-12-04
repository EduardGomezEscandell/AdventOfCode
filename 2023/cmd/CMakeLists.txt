add_executable(aoc2023 main.cpp cmd.cpp app.cpp)
set_target_properties(aoc2023 PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(aoc2023 INTERFACE ..)
target_link_libraries(aoc2023 PUBLIC solvelib xmaslib TBB::tbb)