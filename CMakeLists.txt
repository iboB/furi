# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.10)

project(furi)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # dev_mode is used below to make life simpler for developers
    # it enables some configurations and the defaults for building tests and examples
    # which typically wouldn't be built if confy is a subdirectory of another project
    set(dev_mode ON)
else()
    set(dev_mode OFF)
endif()

option(FURI_BUILD_TESTS "furi: build tests" ${dev_mode})
option(FURI_BUILD_EXAMPLES "furi: build examples" ${dev_mode})
option(FURI_BUILD_SCRATCH "furi: build scratch project for testing and experiments" ${dev_mode})

mark_as_advanced(FURI_BUILD_TESTS FURI_BUILD_EXAMPLES FURI_BUILD_SCRATCH)

if(dev_mode)
    include(./dev.cmake)
endif()

add_subdirectory(code)

if(FURI_BUILD_SCRATCH)
    add_subdirectory(scratch)
endif()

if(FURI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(FURI_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
