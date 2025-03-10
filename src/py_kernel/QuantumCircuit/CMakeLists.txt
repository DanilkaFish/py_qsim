project(quantum)

add_library(${PROJECT_NAME} OBJECT quantumcircuit.cpp)
add_library(sub::${PROJECT_NAME} ALIAS ${PROJECT_NAME})


target_include_directories(${PROJECT_NAME} PUBLIC  ${CM_SOURCE_PATH})