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
  set(GMOCK_INCLUDE_DIR ${source_dir}/googlemock/include)
  ExternalProject_Get_Property(googletest_project binary_dir)
  set(GTEST_LIBRARY gtest)
  set(GTEST_LIBRARY_PATH ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}${GTEST_LIBRARY}.a)
  add_library(${GTEST_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION ${GTEST_LIBRARY_PATH})
  add_dependencies(${GTEST_LIBRARY} googletest_project)
  set(GTEST_MAIN_LIBRARY ${GTEST_LIBRARY}_main)
  set(GTEST_MAIN_LIBRARY_PATH ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}${GTEST_MAIN_LIBRARY}.a)
  add_library(${GTEST_MAIN_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${GTEST_MAIN_LIBRARY} PROPERTY IMPORTED_LOCATION ${GTEST_MAIN_LIBRARY_PATH})
  add_dependencies(${GTEST_MAIN_LIBRARY} googletest_project)
  set(GMOCK_LIBRARY gmock)
  set(GMOCK_LIBRARY_PATH ${binary_dir}/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}${GMOCK_LIBRARY}.a)
  add_library(${GMOCK_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${GMOCK_LIBRARY} PROPERTY IMPORTED_LOCATION ${GMOCK_LIBRARY_PATH})
  add_dependencies(${GMOCK_LIBRARY} googletest_project)
endif(BUILD_TESTING)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/protocol)
if(ANDROID)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/database-server/data-model)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/database-server/client)
else(ANDROID)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/database-server)
endif(ANDROID)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hmi)


if(BUILD_DOC)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/doc)
endif(BUILD_DOC)
