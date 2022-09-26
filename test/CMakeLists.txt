cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(./get_cpm.cmake)

CPMAddPackage(gh:iboB/doctest-util@0.0.5)
CPMAddPackage(gh:ThrowTheSwitch/Unity@2.5.2)

macro(add_furi_cpp_test test)
    add_doctest_lib_test(${test} furi ${ARGN})
endmacro()

macro(add_furi_c_test test)
    icm_add_test(
        NAME furi-${test}
        TARGET test-furi-${test}
        LIBRARIES
            unity::framework
            furi
        SOURCES
            ${ARGN}
        FOLDER test
    )
endmacro()

add_furi_c_test(c_core t-furi.c)
add_furi_cpp_test(cpp_core t-furi.cpp)

# if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#     set(exe furi-fuzz)
#     add_executable(${exe} f-furi.c)
#     target_link_libraries(${exe} furi)
#     target_compile_options(${exe} PRIVATE -g)
#     target_link_options(${exe} PRIVATE
#         -fsanitize=fuzzer
#         -coverage
#         -fsanitize-coverage=edge,trace-pc-guard,indirect-calls,trace-cmp,trace-div,trace-gep)
#     add_test(NAME ${exe} COMMAND ${exe} --max_total_time=1)
# endif()
