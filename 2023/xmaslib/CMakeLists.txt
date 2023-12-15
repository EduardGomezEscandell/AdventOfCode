add_library(xmaslib
    solution/solution.cpp
    registry/registry.cpp
    log/log.cpp
    matrix/text_matrix.cpp
)

set_target_properties(xmaslib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(xmaslib INTERFACE ..)
