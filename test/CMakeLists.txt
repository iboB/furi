cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(./get_cpm.cmake)

CPMAddPackage(gh:iboB/icm@1.3.3)
CPMAddPackage(gh:iboB/doctest-lib@2.4.6)

include(${icm_SOURCE_DIR}/icm_testing.cmake)

macro(add_furi_test test)
    icm_add_doctest_lib_test(${test} furi ${ARGN})
endmacro()

add_furi_test(split t-split.cpp)
add_furi_test(authority_split t-authority_split.cpp)
