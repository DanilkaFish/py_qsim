cmake_minimum_required(VERSION 3.14)
# project(cmake_example)
set(PY_VERSION_SUFFIX "")
set(PY_FULL_VERSION ${PROJECT_VERSION}${PY_VERSION_SUFFIX})

# add_subdirectory(pybind11)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_Declare(
			pybind11
			GIT_REPOSITORY https://github.com/pybind/pybind11
		)
find_package(pybind11 PATHS ${CMAKE_SOURCE_DIR}/_deps)
FetchContent_GetProperties(pybind11)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(cpp_kernel MODULE "${CMAKE_CURRENT_SOURCE_DIR}/utils_wrapper.cpp" )
target_link_libraries(cpp_kernel PUBLIC src)

if (NOT DEFINED PY_BUILD_CMAKE_IMPORT_NAME)
	set(PY_BUILD_CMAKE_IMPORT_NAME "py_qsim")
endif()

install(TARGETS cpp_kernel
        EXCLUDE_FROM_ALL
        COMPONENT python_modules
        DESTINATION ${PY_BUILD_CMAKE_IMPORT_NAME})

set_target_properties(cpp_kernel PROPERTIES
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}${PYTHON_MODULE_DEBUG_POSTFIX}")
target_compile_definitions(cpp_kernel PRIVATE
    MODULE_NAME=$<TARGET_FILE_BASE_NAME:cpp_kernel>
    VERSION_INFO="${PY_FULL_VERSION}"
)
# Hide all symbols by default (including external libraries on Linux)
set_target_properties(cpp_kernel PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN true)
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_options(cpp_kernel PRIVATE "LINKER:--exclude-libs,ALL")
endif()


# Generate stubs for the Python module
set(WITH_PY_STUBS_DEFAULT On)
if (CMAKE_CROSSCOMPILING)
    set(WITH_PY_STUBS_DEFAULT Off)
endif()
option(WITH_PY_STUBS
    "Generate Python stub files (.pyi) for the Python module."
    ${WITH_PY_STUBS_DEFAULT})
if (WITH_PY_STUBS)
    include(../cmake/Pybind11Stubgen.cmake)
    pybind11_stubgen(cpp_kernel
        PACKAGE ${PY_BUILD_CMAKE_IMPORT_NAME}
        COMPONENT python_stubs)
endif()