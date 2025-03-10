cmake_minimum_required(VERSION 3.14)
# project(cmake_example)

# add_subdirectory(pybind11)
FetchContent_Declare(
			pybind11
			GIT_REPOSITORY https://github.com/pybind/pybind11
		)
FetchContent_GetProperties(pybind11)
FetchContent_MakeAvailable(pybind11)

pybind11cpp_kernel_module(cpp_kernel_module MODULE "${CMAKE_CURRENT_SOURCE_DIR}/utils_wrapper.cpp")

if (NOT DEFINED PY_BUILD_CMAKE_IMPORT_NAME)
	set(PY_BUILD_CMAKE_IMPORT_NAME "py_kernel")
endif()

install(TARGETS cpp_kernel_module
        EXCLUDE_FROM_ALL
        COMPONENT python_modules
        DESTINATION ${PY_BUILD_CMAKE_IMPORT_NAME})