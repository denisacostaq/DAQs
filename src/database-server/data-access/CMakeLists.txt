add_library(dataaccess ${CMAKE_CURRENT_SOURCE_DIR}/dataaccess.cc)
target_link_libraries(dataaccess PUBLIC sqlitedatasource)

if(BUILD_TESTING)
  add_executable(dataaccess_test
    ${CMAKE_CURRENT_SOURCE_DIR}/test/dataaccesstest.cc)
  target_include_directories(dataaccess_test SYSTEM PRIVATE ${GTEST_INCLUDE_DIR})
  target_link_libraries(dataaccess_test PRIVATE ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} dataaccess)
  gtest_add_tests(dataaccess_test "" AUTO)
  if(ENABLE_CODE_CEVERAGE_REPORT)
    add_dependencies(coverage dataaccess_test)
  endif(ENABLE_CODE_CEVERAGE_REPORT)
endif(BUILD_TESTING)
