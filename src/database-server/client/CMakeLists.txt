find_package(Qt5 5.11.3 COMPONENTS Core Network REQUIRED)

set(CMAKE_AUTOMOC ON)
add_library(database-client SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/client.cc)
target_link_libraries(database-client PUBLIC proto-messages datamodel Qt5::Core Qt5::Network)
