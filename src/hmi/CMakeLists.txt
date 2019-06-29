set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 5.11.3 COMPONENTS Core Quick Charts REQUIRED)

add_executable(hmi
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc)
target_compile_definitions(
  hmi PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(hmi PRIVATE Qt5::Core Qt5::Quick Qt5::Charts)
