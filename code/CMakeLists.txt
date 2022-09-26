add_library(furi INTERFACE)
add_library(furi::furi ALIAS furi)
target_include_directories(furi INTERFACE .)
