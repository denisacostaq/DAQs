add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data-model)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data-access)

find_package(Boost 1.67 REQUIRED system thread)
add_executable(database-server
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cc)
target_include_directories(database-server PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(database-server PRIVATE proto-messages ${Boost_LIBRARIES})
