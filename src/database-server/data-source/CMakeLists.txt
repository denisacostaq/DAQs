set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_program(MAKE_EXE make)
if(NOT MAKE_EXE)
    message(SEND_ERROR "Make executable is required to build sqlite3.")
endif(NOT MAKE_EXE)

find_program(GCC_EXE gcc)
if(NOT GCC_EXE)
  message(SEND_ERROR "gcc executable is required to build sqlite3.")
endif(NOT GCC_EXE)
ExternalProject_Add(sqlite3_project
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/sqlite3
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sqlite3
    URL https://www.sqlite.org/snapshot/sqlite-snapshot-202003121754.tar.gz
    CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/3rdparty/sqlite3/configure --enable-threadsafe=true CC=${GCC_EXE}
    BUILD_COMMAND ${MAKE_EXE} -C ${CMAKE_CURRENT_BINARY_DIR}/sqlite3 -j${N_PROC}
    INSTALL_COMMAND echo "Skipping install sqlite3"
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

add_library(sqlitedatasource
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlitewrapper.cc)
target_include_directories(sqlitedatasource SYSTEM PUBLIC ${SQLITE3_INCLUDE_DIR})
target_link_libraries(sqlitedatasource PUBLIC datamodel ${SQLITE3_LIBRARY} Threads::Threads ${CMAKE_DL_LIBS})

if(SQLITEDATASOURCE_TEST_ENABLED)
  find_package(Boost 1.67 REQUIRED filesystem)
  add_library(test_util ${CMAKE_CURRENT_SOURCE_DIR}/test/testutil.cc)
  target_include_directories(test_util SYSTEM PUBLIC ${Boost_INCLUDE_DIR})
  target_link_libraries(test_util PUBLIC ${Boost_LIBRARIES})
  add_executable(sqlitedatasource_test
    ${CMAKE_CURRENT_SOURCE_DIR}/test/sqlitedatasource_integration_test.cc)
  target_include_directories(sqlitedatasource_test SYSTEM PRIVATE ${GTEST_INCLUDE_DIR})
  target_link_libraries(sqlitedatasource_test PRIVATE ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} sqlitedatasource test_util)
  gtest_add_tests(sqlitedatasource_test "" AUTO)
  if(ENABLE_CODE_CEVERAGE_REPORT)
    add_dependencies(coverage sqlitedatasource_test)
  endif(ENABLE_CODE_CEVERAGE_REPORT)
endif(SQLITEDATASOURCE_TEST_ENABLED)
