cmake_minimum_required(VERSION 3.24)

project(cpp_kernel VERSION 0.1.0 )


##### Check compiler Version #####
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_C_COMPILER_VERSION = ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")


# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

##### configure include files #####
file(GLOB_RECURSE header_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
foreach(path IN LISTS header_files)
	string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/src/ ${CMAKE_CURRENT_SOURCE_DIR}/include/ path_dst ${path})
	configure_file(${path} ${path_dst} COPYONLY)
endforeach()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)


include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/_deps)

if(USE_PYTHON)
	add_subdirectory(python_wrapper)
else()
	message(STATUS "Skip downloading pybind11")
endif()


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)


if(USE_TEST)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
else()
	message(STATUS "Skip tests")
endif()


