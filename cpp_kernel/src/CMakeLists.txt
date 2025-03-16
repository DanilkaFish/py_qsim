add_library(src)
target_include_directories(src INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Utils)
# add_subdirectory(quantumCircuit)

