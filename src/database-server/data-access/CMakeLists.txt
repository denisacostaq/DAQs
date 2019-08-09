add_library(dataaccess ${CMAKE_CURRENT_SOURCE_DIR}/dataaccess.cc)
target_link_libraries(dataaccess PUBLIC sqlitedatasource)

if(BUILD_TESTING)
  add_executable(dataaccess_test
    ${CMAKE_CURRENT_SOURCE_DIR}/test/dataaccesstest.cc)
  target_include_directories(dataaccess_test SYSTEM PRIVATE ${GTEST_INCLUDE_DIR})
  target_link_libraries(dataaccess_test PRIVATE
    dataaccess test_util ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
  gtest_add_tests(dataaccess_test "" AUTO)
  if(ENABLE_CODE_CEVERAGE_REPORT)
    add_dependencies(coverage dataaccess_test)
  endif(ENABLE_CODE_CEVERAGE_REPORT)

  add_executable(dataaccess_unit_test
    ${CMAKE_CURRENT_SOURCE_DIR}/test/dataaccess_unit_test.cc)
  target_include_directories(dataaccess_unit_test SYSTEM PRIVATE
    ${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR})
  target_link_libraries(dataaccess_unit_test PRIVATE
    dataaccess ${GMOCK_LIBRARY} ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
  gtest_add_tests(dataaccess_unit_test "" AUTO)
  if(ENABLE_CODE_CEVERAGE_REPORT)
    add_dependencies(coverage dataaccess_test)
  endif(ENABLE_CODE_CEVERAGE_REPORT)

endif(BUILD_TESTING)
