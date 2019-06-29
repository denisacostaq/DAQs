include(ExternalProject)
if(BUILD_TESTING)
  include(GoogleTest)
  ExternalProject_Add(googletest_project
      SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/gtest
      BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/gtest
      URL https://github.com/google/googletest/archive/release-1.8.1.tar.gz
      INSTALL_COMMAND echo "Skipping install"
  )
  ExternalProject_Get_Property(googletest_project source_dir)
  set(GTEST_INCLUDE_DIR ${source_dir}/googletest/include)
  ExternalProject_Get_Property(googletest_project binary_dir)
  set(GTEST_LIBRARY gtest)
  set(GTEST_LIBRARY_PATH ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}${GTEST_LIBRARY}.a)
  add_library(${GTEST_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION ${GTEST_LIBRARY_PATH} )
  add_dependencies(${GTEST_LIBRARY} googletest_project)
  set(GTEST_MAIN_LIBRARY ${GTEST_LIBRARY}_main)
  set(GTEST_MAIN_LIBRARY_PATH ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}${GTEST_MAIN_LIBRARY}.a)
  add_library(${GTEST_MAIN_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${GTEST_MAIN_LIBRARY} PROPERTY IMPORTED_LOCATION ${GTEST_MAIN_LIBRARY_PATH})
  add_dependencies(${GTEST_MAIN_LIBRARY} googletest_project)
endif(BUILD_TESTING)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/protocol)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/database-server)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hmi)
