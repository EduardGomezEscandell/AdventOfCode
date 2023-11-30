add_library(xmaslib
    solvebase/solvebase.hpp
    registry/registry.hpp registry/registry.cpp
)

set_target_properties(xmaslib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(xmaslib INTERFACE ..)
