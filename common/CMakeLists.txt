add_library(file_utils file_utils.c file_utils.h)
set_target_properties(file_utils PROPERTIES LINKER_LANGUAGE C)
target_include_directories(file_utils INTERFACE ..)

add_library(matrix matrix.c matrix.h)
set_target_properties(matrix PROPERTIES LINKER_LANGUAGE C)
target_include_directories(matrix INTERFACE ..)

add_library(timing timing.c timing.h)
set_target_properties(timing PROPERTIES LINKER_LANGUAGE C)
target_include_directories(timing INTERFACE ..)

add_library(forward_list forward_list.c forward_list.h)
set_target_properties(forward_list PROPERTIES LINKER_LANGUAGE C)
target_include_directories(forward_list INTERFACE ..)