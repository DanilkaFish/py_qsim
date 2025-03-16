find_package(GTest REQUIRED)
link_libraries(${GTEST_LIBRARIES} gtest_main pthread)


set(TEST_QC_DIR "QuantumCircuit")
set(TEST_TENSOR_DIR "Tensors")
set(TEST_SIM_DIR "GateSimulator")

# option(TENSOR_TEST "Tensor" ON)

include(Utils/CMakeLists.txt)

