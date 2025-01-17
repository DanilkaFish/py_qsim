cmake_minimum_required(VERSION 3.14)

project(quanlib VERSION 0.1.0 )


option(TESTS "enable tests" ON)
option(MAIN "grover" ON)

set(TR_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src/Tensors)
set(QC_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src/QuantumCircuit)
set(GS_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src/StateSimulator)
set(QS_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src/QiskitSimulator)
set(RD_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src/random)
set(CM_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src/Utils)
set(VQE_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src/VQE)
set(QULACS_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/../ext_sources/qulacs/include)
set(NLOPT_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/../ext_sources/nlopt/bin)
set(EIGEN_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/../ext_sources/Eigen/)
# set(MYPUBLIC_PATH ${PROJECT_SOURCE_DIR}/)

# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


file(GLOB_RECURSE header_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
foreach(path IN LISTS header_files)
	string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/src/ ${CMAKE_CURRENT_SOURCE_DIR}/include/ path_dst ${path})
	configure_file(${path} ${path_dst} COPYONLY)
endforeach()
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

# set(CMAKE_INCLUDE_CURRENT_DIR True)


# include_directories(include/QuantumCircuit)
# include_directories(include/QuantumCircuit/algorithms)
# include_directories(include/QuantumCircuit/algorithms)

# set(CMAKE_PREFIX_PATH "~/python_venvs/model/bin/python3")
# list(APPEND CMAKE_PREFIX_PATH "/usr/lib/python3.12")
# # /home/yadanilka/python_venvs/model/lib
# set(Python3_FIND_VIRTUALENV FIRST)
# find_package(Python REQUIRED COMPONENTS Interpreter Development)
# # find_package(PythonLibs REQUIRED)
# message(NOTICE ${Python_STDLIB})
# set(PYTHON_INCLUDE_DIRS "/usr/lib/python3.12")
# message(NOTICE ${PYTHON_INCLUDE_DIRS})
# message(NOTICE ${PYTHON_LIBRARIES})
# # set(Python_LIBRARY "/usr/lib/python3.12")
# # message(NOTICE ${CMAKE_PREFIX_PATH})
# # message(NOTICE "this message is on stderr pipe")
# # cmake_print_variables(PYTHON_INCLUDE_DIRS)
# link_libraries(${PYTHON_LIBRARIES})
# -------------------------------------------------
# find_package (Python3 REQUIRED COMPONENTS Interpreter Development )
# set(Python3_INCLUDE_DIRS /usr/include/python3.12)
# include_directories(${Python3_INCLUDE_DIRS})

# message(STATUS "Found Python: ${Python3_FOUND} ${Python3_INTERPRETER_ID} ${Python3_EXECUTABLE}")
# message(STATUS "Found Python3_LIBRARIES: ${Python3_LIBRARIES}")
# message(STATUS "Found INCLUDE: ${Python3_INCLUDE_DIRS}")
# message(STATUS "Found Python3_Development_FOUND: ${Python3_Development_FOUND}")
# message(STATUS "Found Python3_NumPy_FOUND: ${Python3_NumPy_FOUND} ${Python3_NumPy_INCLUDE_DIRS}")
# include_directories(${PROJECT_SOURCE_DIR}/include/quantumcircuit)
# include_directories(${PROJECT_SOURCE_DIR}/include/Tensors)
# include_directories(${PROJECT_SOURCE_DIR}/include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../lib)


add_subdirectory(src/random)
add_subdirectory(src/Utils)
add_subdirectory(src/QuantumCircuit)
add_subdirectory(${GS_SOURCE_PATH})
add_subdirectory(src/Tensors)
add_subdirectory(src/algorithms)
add_subdirectory(src/VQE)
add_subdirectory(src/QiskitSimulator)

add_library(quan)
# target_link_libraries(quan PUBLIC sub::quantum sub::tensor sub::qubits sub::state_evolve sub::grover sub::random )
target_link_libraries(quan PUBLIC quantum tensor common state_evolve grover random random_qc vqe qiskit_evolve qaoa)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# add_library(everything)
# target_link_libraries(everything PUBLIC sub::quantum sub::tensor sub::qubits sub::state_evolve sub::grover sub::random)


if(TESTS)
    add_subdirectory(tests)
else()
endif()


# if(MAIN)
#     add_executable(main)
#     target_sources(main PRIVATE 
#         main.cpp
#         )
#     target_link_libraries(main PRIVATE sub::quantum sub::tensor sub::qubits sub::state_evolve sub::grover sub::random )
#     target_include_directories(main PUBLIC ${TR_SOURCE_PATH} ${MYPUBLIC_PATH})
#     target_compile_options(main PRIVATE -pthread)
#     # target_link_options(main PRIVATE -pthread -sPTHREAD_POOL_SIZE=16)
# else()
# endif()

