if (BUILD_TESTS)
    set(ENABLE_DOCTESTS 1)

    add_executable(test test.cpp)

    set_target_properties(test PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(test INTERFACE ..)

    target_link_libraries(test PUBLIC doctest solvelib xmaslib)
endif()