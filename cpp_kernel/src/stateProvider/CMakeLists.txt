target_include_directories(src
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                           )
target_sources(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source.cpp)
# target_include_directories(src INTERFACE source)
# add_library(sub::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
