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
include(ProcessorCount)
ProcessorCount(N_PROC)
if(N_PROC EQUAL 0)
    set(N_PROC 1)
endif(N_PROC EQUAL 0)
set(CMAKE_BUILD_PARALLEL_LEVEL ${N_PROC})
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ClangFormat.cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

option(BUILD_TESTING OFF)
if(BUILD_TESTING)
    include(CTest)
endif(BUILD_TESTING)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
