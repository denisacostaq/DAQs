set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_program(MAKE_EXE make)
if(NOT MAKE_EXE)
    message(SEND_ERROR "Make executable is required to build sqlite3.")
endif(NOT MAKE_EXE)

include(ExternalProject)
ExternalProject_Add(sqlite_project
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/sqlite3
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sqlite3
    URL https://www.sqlite.org/snapshot/sqlite-snapshot-201906171413.tar.gz
    CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/3rdparty/sqlite3/configure
    BUILD_COMMAND ${MAKE_EXE} -C ${CMAKE_CURRENT_BINARY_DIR}/sqlite3 -j${N_PROC}
    INSTALL_COMMAND echo "Skipping install"
)
add_library(sqlite STATIC IMPORTED)
add_dependencies(sqlite sqlite_project)
set_property(TARGET sqlite
    PROPERTY IMPORTED_LOCATION 
    "${CMAKE_CURRENT_BINARY_DIR}/sqlite3/.libs/libsqlite3.a")

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)
if(NOT CMAKE_USE_PTHREADS_INIT)
     message(FATAL_ERROR "pthread it's required")
endif(NOT CMAKE_USE_PTHREADS_INIT)

add_library(sqlitedatamodel
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlitewrapper.cc)
target_include_directories(sqlitedatamodel PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/sqlite3)
target_link_libraries(sqlitedatamodel sqlite Threads::Threads ${CMAKE_DL_LIBS})
