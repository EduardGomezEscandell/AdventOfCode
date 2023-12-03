add_library(xmaslib
    solution/solution.hpp solution/solution.cpp
    registry/registry.hpp registry/registry.cpp
    log/log.hpp
)

set_target_properties(xmaslib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(xmaslib INTERFACE ..)
