# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
function(add_x name)
    set(tgt furi-${name})
    add_executable(${tgt} ${ARGN})
    target_link_libraries(${tgt} PRIVATE furi::furi)
endfunction()

add_x(decompose decompose-cli.c)
