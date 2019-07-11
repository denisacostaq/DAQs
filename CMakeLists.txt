cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(VERION_MAJOR CACHE 0 INTERNAL)
set(VERION_MINOR CACHE 0 INTERNAL)
set(VERION_PATCH CACHE 0 INTERNAL)

#TODO(denisacostaq@gmail.com)
#VERSION ${VERION_MAJOR}.${VERION_MINOR}.${VERION_PATCH}
# new policy CMP0048
project(DAQ
    DESCRIPTION "Data Acquisition System (basic demo)"
    HOMEPAGE_URL "https://github.com/denisacostaq/DAQs"
    LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
option(BUILD_DOC "Build documenation" OFF)
if(BUILD_DOC)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxygen.cmake)
endif(BUILD_DOC)
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

cmake_dependent_option(ENABLE_CODE_CEVERAGE_REPORT
    "Enable code coverage report." OFF
    "BUILD_TESTING" ON)
if(ENABLE_CODE_CEVERAGE_REPORT)
    get_filename_component(COMPILER_PATH ${CMAKE_CXX_COMPILER} PATH)
    string(REGEX REPLACE "\\..*" "" COMPILER_MAJOR_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
#        hint COMPILER_PATH
        find_program(COV_EXE llvm-cov-${COMPILER_MAJOR_VERSION})
        find_program(LLVM_PROFDATA_EXE llvm-profdata-${COMPILER_MAJOR_VERSION})
        if(NOT LLVM_PROFDATA_EXE)
            message(SEND_ERROR "llvm profdata tool is missing.")
        endif(NOT LLVM_PROFDATA_EXE)
    elseif(CMAKE_COMPILER_IS_GNUCXX)
#        hint COMPILER_PATH
        find_program(COV_EXE gcov-${COMPILER_MAJOR_VERSION})
    else()
        message(SEND_ERROR "Code coverage is supported for clang and gcc only in this project.")
    endif()
    if(NOT COV_EXE)
        message(SEND_ERROR "Code coverage tool is missing.")
    endif(NOT COV_EXE)
    file(GLOB_RECURSE
      ALL_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
    add_custom_target(coverage
        COMMAND mkdir -p coverage
        COMMAND ${CMAKE_MAKE_PROGRAM} test
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    if(CMAKE_COMPILER_IS_GNUCXX)
        SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
        SET(CMAKE_C_FLAGS "-g -O0 -W -fprofile-arcs -ftest-coverage")
        SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
        add_custom_command(TARGET coverage
            COMMENT "Generating coverage files"
            COMMAND ${COV_EXE} -b ${ALL_SOURCES} -o ${CMAKE_BINARY_DIR} | > CoverageSummary.tmp
            COMMAND cat CoverageSummary.tmp
            BYPRODUCTS CoverageSummary.tmp
            COMMAND echo "Coverage files have been output to ${CMAKE_BINARY_DIR}/coverage"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
        )
        add_custom_command(TARGET coverage
            COMMENT "Generating coverage info"
            COMMAND lcov -c -d ${CMAKE_BINARY_DIR}/src/database-server/data-model/ -o coverage.info --ignore-errors source
            COMMAND echo "Coverage info have been output to a.info"
            BYPRODUCTS coverage.info
            MAIN_DEPENDENCY CoverageSummary.tmp
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
        )
        add_custom_command(TARGET coverage
            COMMENT "Generating html coverage"
            COMMAND genhtml --title "Code coverage report for DAQs." coverage.info --output-directory html
            COMMAND echo "Coverage info have been output to d"
            MAIN_DEPENDENCY a.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
        )
        #FIXME(denisacostaq@gmail.com): Make sure to clean up the coverage folder
        # fix for clang too
#        file(GLOB_RECURSE
#          COVER_FILES
#          ${CMAKE_BINARY_DIR}/src/*.gcda
#          ${CMAKE_BINARY_DIR}/src/*.gcno)
#        foreach(COVER_FILE IN "${COVER_FILES}")
#            set_property(FILE APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${COVER_FILE})
#        endforeach(COVER_FILE IN "${COVER_FILES}")
#        # Create the coverage-clean target. This cleans the build as well as generated 
#        # .gcda and .gcno files.
#        add_custom_target(scrub
#            COMMAND ${CMAKE_MAKE_PROGRAM} clean
#            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#        )
    #set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES coverage)
    else(CMAKE_COMPILER_IS_GNUCXX)
        SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-instr-generate -fcoverage-mapping")
        SET(CMAKE_C_FLAGS "-g -O0 -W -fprofile-instr-generate -fcoverage-mapping")
        SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
        add_custom_command(TARGET coverage
            COMMENT "Indexing prof raw data"
            COMMAND ${LLVM_PROFDATA_EXE} merge -sparse ${CMAKE_BINARY_DIR}/src/database-server/data-model/default.profraw -o ${CMAKE_BINARY_DIR}/default.profdata
            BYPRODUCTS ${CMAKE_BINARY_DIR}/default.profdata
            MAIN_DEPENDENCY ${CMAKE_BINARY_DIR}/src/database-server/data-model/default.profraw
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
        )
        add_custom_command(TARGET coverage
            COMMENT "Generating coverage files"
            COMMAND ${COV_EXE} show -format=html -instr-profile=${CMAKE_BINARY_DIR}/default.profdata ${CMAKE_BINARY_DIR}/src/database-server/data-model/sqlitedatamodel_test ${ALL_SOURCES} -output-dir html
            COMMAND echo "Coverage files have been output to ${CMAKE_BINARY_DIR}/coverage"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
        )
    endif(CMAKE_COMPILER_IS_GNUCXX)
endif(ENABLE_CODE_CEVERAGE_REPORT)
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
