
project(test_utils)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/utils.cpp)
message(STATUS "path2=${source_path}")
target_include_directories(${PROJECT_NAME} PUBLIC ${source_path})

target_link_libraries(${PROJECT_NAME} utils)
# target_sources(${PROJECT_NAME}  )
