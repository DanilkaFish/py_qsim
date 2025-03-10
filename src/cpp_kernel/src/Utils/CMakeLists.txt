add_library(utils OBJECT source.cpp)
target_include_directories(utils
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                           )

target_link_libraries(utils PUBLIC src)
# add_library(sub::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
