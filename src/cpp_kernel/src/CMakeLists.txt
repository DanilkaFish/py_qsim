add_library(src INTERFACE)
target_include_directories(src INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "src current=${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(Utils)
