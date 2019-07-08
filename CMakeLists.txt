cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(VERION_MAJOR CACHE 0 INTERNAL)
set(VERION_MINOR CACHE 0 INTERNAL)
set(VERION_PATCH CACHE 0 INTERNAL)

#TODO(denisacostaq@gmail.com)
#VERSION ${VERION_MAJOR}.${VERION_MINOR}.${VERION_PATCH}
# new policy CMP0048
project(DAQ
#    DESCRIPTION "Data Acquisition System (basic demo)"
#    HOMEPAGE_URL "https://github.com/denisacostaq/DAQs"
    LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
if(ANDROID)
    if(NOT DEFINED Qt5SDK_DIR)
        message(FATAL_ERROR "Qt5SDK_DIR is required.")
    endif(NOT DEFINED Qt5SDK_DIR)
    set(Qt5_DIR ${Qt5SDK_DIR}/lib/cmake/Qt5)
    set(Qt5Core_DIR ${Qt5SDK_DIR}/lib/cmake/Qt5Core)
    set(Qt5Network_DIR ${Qt5SDK_DIR}/lib/cmake/Qt5Network)
    set(Qt5Quick_DIR ${Qt5SDK_DIR}/lib/cmake/Qt5Quick)
    set(Qt5Gui_DIR ${Qt5SDK_DIR}/lib/cmake/Qt5Gui)
    set(Qt5Qml_DIR ${Qt5SDK_DIR}/lib/cmake/Qt5Qml)
    set(Qt5Charts_DIR ${Qt5SDK_DIR}/lib/cmake/Qt5Charts)
    set(Qt5Widgets_DIR ${Qt5SDK_DIR}/lib/cmake/Qt5Widgets)
    if(NOT DEFINED ANDROID_SDK_DIR)
        message(FATAL_ERROR "ANDROID_SDK_DIR is required.")
    endif(NOT DEFINED ANDROID_SDK_DIR)
    set(ANDROID_TOOLCHAIN_MACHINE_NAME llvm)
    set(ANDROID_COMPILER_VERSION 4.9)
endif(ANDROID)
include(ProcessorCount)
ProcessorCount(N_PROC)
if(N_PROC EQUAL 0)
    set(N_PROC 1)
endif(N_PROC EQUAL 0)
set(CMAKE_BUILD_PARALLEL_LEVEL ${N_PROC})
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ClangFormat.cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

option(BUILD_TESTING "Build tests" OFF)
include(CMakeDependentOption)
cmake_dependent_option(SQLITEDATAMODEL_TEST_ENABLED
	"Build sqlite datamodel integration tests" ON
	"BUILD_TESTING" ON)

option(SQLITEDATAMODEL_TEST_ENABLED ON)
if(BUILD_TESTING)
    include(CTest)
endif(BUILD_TESTING)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

if(ANDROID)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/android/configAndroid.json.in
      ${CMAKE_CURRENT_BINARY_DIR}/configAndroid.json @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml.in
      ${CMAKE_CURRENT_BINARY_DIR}/android-build/AndroidManifest.xml @ONLY)
    # check if the apk must be installed to the device
    if(ARG_INSTALL)
        set(INSTALL_OPTIONS --reinstall)
    endif(ARG_INSTALL)
    add_custom_target(apk_debug
        COMMENT "Building android apk"
        DEPENDS hmi
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI} # it seems that recompiled libraries are not copied if we don't remove them first
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/android-build/libs/${ANDROID_ABI}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/src/hmi/libhmi.so ${CMAKE_CURRENT_BINARY_DIR}/android-build/libs/${ANDROID_ABI}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/src/protocol/libproto-messages.so ${CMAKE_CURRENT_BINARY_DIR}/android-build/libs/${ANDROID_ABI}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/src/database-server/client/libdatabase-client.so ${CMAKE_CURRENT_BINARY_DIR}/android-build/libs/${ANDROID_ABI}
        COMMAND ${CMAKE_COMMAND} -E copy ${Protobuf_DIR}/../../libprotobuf.so ${CMAKE_CURRENT_BINARY_DIR}/android-build/libs/${ANDROID_ABI}
        COMMAND ${Qt5SDK_DIR}/bin/androiddeployqt --verbose --output ${CMAKE_CURRENT_BINARY_DIR}/android-build --input ${CMAKE_CURRENT_BINARY_DIR}/configAndroid.json --android-platform android-21 --gradle ${INSTALL_OPTIONS}
     )
 #./gradlew build --debug
endif(ANDROID)
