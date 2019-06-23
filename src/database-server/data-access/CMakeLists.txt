add_library(dataaccess ${CMAKE_CURRENT_SOURCE_DIR}/dataaccess.cc)
target_link_libraries(dataaccess PUBLIC sqlitedatamodel)

add_executable(main ${CMAKE_CURRENT_SOURCE_DIR}/main.cc)
target_link_libraries(main PRIVATE dataaccess)
