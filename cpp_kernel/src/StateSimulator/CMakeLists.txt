project(state_evolve)

add_library(${PROJECT_NAME} OBJECT gatesimulator.cpp)
add_library(sub::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${TR_SOURCE_PATH} ${QC_SOURCE_PATH} ${CM_SOURCE_PATH} )


