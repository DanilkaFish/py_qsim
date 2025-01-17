project(vqe)

add_library(${PROJECT_NAME} OBJECT vqe.cpp)
add_library(sub::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${QC_SOURCE_PATH} ${CM_SOURCE_PATH} ${NLOPT_INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME} PUBLIC ${NLOPT_INCLUDE_PATH}/libnlopt.so)
# export(TARGETS qubits)