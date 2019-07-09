set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 5.11.3 COMPONENTS Core Quick Charts REQUIRED)

set(HMI_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/historicdata.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc)
if(ANDROID)
    add_library(hmi SHARED ${HMI_SRC})
else(ANDROID)
    add_executable(hmi ${HMI_SRC})
endif(ANDROID)
target_compile_definitions(
  hmi PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(hmi PRIVATE database-client Qt5::Core Qt5::Quick Qt5::Charts)
