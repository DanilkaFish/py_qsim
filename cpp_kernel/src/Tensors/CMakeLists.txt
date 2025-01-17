project(tensor)

add_library(${PROJECT_NAME} OBJECT tensors.cpp)
add_library(sub::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${QC_SOURCE_PATH} ${CM_SOURCE_PATH} )

# export(tensor)