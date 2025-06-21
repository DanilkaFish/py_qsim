add_library(src)
target_include_directories(src INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Utils)
add_subdirectory(stateProvider)
add_subdirectory(newTensors)

# add_subdirectory(quantumCircuit)

