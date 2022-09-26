# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(USE_ASAN "furi-dev: build with address sanitizer on" OFF)
option(USE_CLANG_TIDY "furi-dev: use clang tidy" OFF)

set(DEV_SAN_FLAGS "")
if(MSVC)
    set(DEV_WARNING_FLAGS "-D_CRT_SECURE_NO_WARNINGS=1 /Zc:__cplusplus /w34100 /permissive-")
else()
    set(DEV_WARNING_FLAGS "-Wall -Wextra -Werror=return-type")
    if(USE_ASAN)
        set(DEV_SAN_FLAGS "-fsanitize=address,undefined,leak -g")
    endif()

    if(USE_CLANG_TIDY)
        set(CMAKE_CXX_CLANG_TIDY clang-tidy)
    endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DEV_WARNING_FLAGS} ${DEV_SAN_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEV_WARNING_FLAGS} ${DEV_SAN_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${DEV_SAN_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${DEV_SAN_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
