cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(./get_cpm.cmake)

CPMAddPackage(gh:iboB/icm@1.3.3)
CPMAddPackage(gh:iboB/doctest-lib@2.4.6)
CPMAddPackage(gh:ThrowTheSwitch/Unity@2.5.2)

include(${icm_SOURCE_DIR}/icm_testing.cmake)

macro(add_furi_test test)
    icm_add_doctest_lib_test(${test} furi ${ARGN})
endmacro()

macro(add_furi_c_test test)
    icm_add_test(
        NAME furi-${test}
        TARGET test-furi-${test}
        LIBRARIES
            unity
            furi
        SOURCES
            ${ARGN}
        FOLDER test
    )
endmacro()

# No tests for strutil as it copied from iboB/itlib. It's tested there

add_furi_c_test(c_string_view_c t-c_string_view.c)

add_furi_test(split t-split.cpp)
add_furi_test(authority_split t-authority_split.cpp)
add_furi_test(path_iterator t-path_iterator.cpp)
add_furi_test(common_query_iterator t-common_query_iterator.cpp)
