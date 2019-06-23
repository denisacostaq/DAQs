add_library(dataaccess ${CMAKE_CURRENT_SOURCE_DIR}/dataaccess.cc)
target_link_libraries(dataaccess PUBLIC sqlitedatamodel)
