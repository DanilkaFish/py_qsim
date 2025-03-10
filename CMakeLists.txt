cmake_minimum_required(VERSION 3.24)
project(py_qsim VERSION 0.1.0 )


##### Set behavior #####
option(USE_TEST "enable tests" OFF)
option(USE_PYTHON "download python" on)



message(STATUS "USE_TEST=${USE_TEST}")
message(STATUS "USE_PYTHON=${USE_PYTHON}")
message(STATUS "source pathes=${source_path}")
message(STATUS "module path=${CMAKE_PREFIX_PATH}")

message(STATUS "------${PY_BUILD_CMAKE_IMPORT_NAME}")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/cpp_kernel)


