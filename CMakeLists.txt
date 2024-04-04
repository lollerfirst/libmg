cmake_minimum_required(VERSION 3.0)
project(MyProject)

find_library(GMP_LIBRARIES gmp)
include_directories(.)

if(NOT GMP_LIBRARIES)
    message(FATAL_ERROR "libgmp is not installed. Please install libgmp.")
endif()

add_library(mg STATIC libmg.c)
set_target_properties(mg PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(test_mg test.c libmg)
target_link_libraries(test_mg ${GMP_LIBRARIES})

add_executable(poc_fhe poc_fhe.c libmg)
target_link_libraries(poc_fhe ${GMP_LIBRARIES})