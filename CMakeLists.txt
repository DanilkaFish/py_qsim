cmake_minimum_required(VERSION 3.24)
project(py_qsim VERSION 0.1.0 )


##### Set behavior #####
option(USE_TEST "enable tests" OFF)
option(USE_PYTHON "download python" on)
option(USE_LOCAL_BUILDING "local build" on)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/py_qsim/cpp_lib)


message(STATUS "USE_TEST=${USE_TEST}")
message(STATUS "USE_PYTHON=${USE_PYTHON}")


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cpp_kernel)


if (NOT USE_PYTHON)
    add_executable(main mymain.cpp)
    target_link_libraries(main PUBLIC src)
endif()