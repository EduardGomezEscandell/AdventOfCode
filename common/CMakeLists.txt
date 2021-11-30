add_library(math math.h)
set_target_properties(math PROPERTIES LINKER_LANGUAGE C)
target_include_directories(math INTERFACE ..)

add_library(mystring mystring.c mystring.h)
set_target_properties(mystring PROPERTIES LINKER_LANGUAGE C)
target_include_directories(mystring INTERFACE ..)
target_link_libraries(mystring math)

add_library(vector vector.c vector.h)
set_target_properties(vector PROPERTIES LINKER_LANGUAGE C)
target_include_directories(vector INTERFACE ..)
target_link_libraries(vector math mystring)