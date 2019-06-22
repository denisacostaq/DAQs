set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_program(MAKE_EXE make)
if(NOT MAKE_EXE)
    message(SEND_ERROR "Make executable is required to build sqlite3.")
endif(NOT MAKE_EXE)

include(ExternalProject)
ExternalProject_Add(sqlite3_project
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/sqlite3
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sqlite3
    URL https://www.sqlite.org/snapshot/sqlite-snapshot-201906171413.tar.gz
    CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/3rdparty/sqlite3/configure
    BUILD_COMMAND ${MAKE_EXE} -C ${CMAKE_CURRENT_BINARY_DIR}/sqlite3 -j${N_PROC}
    INSTALL_COMMAND echo "Skipping install"
)
ExternalProject_Get_Property(sqlite3_project source_dir)
set(SQLITE3_INCLUDE_DIR ${source_dir})
ExternalProject_Get_Property(sqlite3_project binary_dir)
set(SQLITE3_LIBRARY sqlite3)
set(SQLITE3_LIBRARY_PATH ${binary_dir}/.libs/${CMAKE_FIND_LIBRARY_PREFIXES}${SQLITE3_LIBRARY}.a)
add_library(${SQLITE3_LIBRARY} STATIC IMPORTED)
add_dependencies(${SQLITE3_LIBRARY} sqlite3_project)
set_property(TARGET ${SQLITE3_LIBRARY} PROPERTY IMPORTED_LOCATION ${SQLITE3_LIBRARY_PATH})

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)
if(NOT CMAKE_USE_PTHREADS_INIT)
     message(FATAL_ERROR "pthread it's required")
endif(NOT CMAKE_USE_PTHREADS_INIT)

add_library(sqlitedatamodel
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlitewrapper.cc)
target_include_directories(sqlitedatamodel PUBLIC ${SQLITE3_INCLUDE_DIR})
target_link_libraries(sqlitedatamodel ${SQLITE3_LIBRARY} Threads::Threads ${CMAKE_DL_LIBS})

if(BUILD_TESTING)
  ExternalProject_Add(googletest_project
      SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/gtest
      BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/gtest
      URL https://github.com/google/googletest/archive/release-1.8.1.tar.gz
      INSTALL_COMMAND echo "Skipping install"
  )
  ExternalProject_Get_Property(googletest_project source_dir)
  set(GTEST_INCLUDE_DIR ${source_dir}/include)
  ExternalProject_Get_Property(googletest_project binary_dir)
  set(GTEST_LIBRARY gtest)
  set(GTEST_LIBRARY_PATH ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}${GTEST_LIBRARY}.a)
  add_library(${GTEST_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION ${GTEST_LIBRARY_PATH} )
  add_dependencies(${GTEST_LIBRARY} googletest_project)
endif(BUILD_TESTING)
