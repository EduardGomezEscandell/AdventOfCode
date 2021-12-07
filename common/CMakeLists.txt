add_library(file_utils file_utils.c file_utils.h)
set_target_properties(file_utils PROPERTIES LINKER_LANGUAGE C)
target_include_directories(file_utils INTERFACE ..)

add_library(math math.h)
set_target_properties(math PROPERTIES LINKER_LANGUAGE C)
target_include_directories(math INTERFACE ..)

add_library(vector vector.c vector.h)
set_target_properties(vector PROPERTIES LINKER_LANGUAGE C)
target_include_directories(vector INTERFACE ..)
target_link_libraries(vector math)

add_library(timing timing.c timing.h)
set_target_properties(timing PROPERTIES LINKER_LANGUAGE C)
target_include_directories(timing INTERFACE ..)