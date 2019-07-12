get_filename_component(COMPILER_PATH ${CMAKE_CXX_COMPILER} PATH)
string(REGEX REPLACE "\\..*" "" COMPILER_MAJOR_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
#        hint COMPILER_PATH
    find_program(COV_EXE
        NAMES llvm-cov-${COMPILER_MAJOR_VERSION} llvm-cov)
    find_program(LLVM_PROFDATA_EXE
        NAMES llvm-profdata-${COMPILER_MAJOR_VERSION} llvm-profdata)
    if(NOT LLVM_PROFDATA_EXE)
        message(SEND_ERROR "llvm profdata tool is missing.")
    endif(NOT LLVM_PROFDATA_EXE)
elseif(CMAKE_COMPILER_IS_GNUCXX)
#        hint COMPILER_PATH
    find_program(COV_EXE gcov-${COMPILER_MAJOR_VERSION})
    find_program(LCOV_EXE lcov)
    if(NOT LCOV_EXE)
        message(SEND_ERROR "Code coverage frontend tool is missing.")
    endif(NOT LCOV_EXE)
    find_program(GENHTML_EXE genhtml)
    if(NOT GENHTML_EXE)
        message(SEND_ERROR "Code coverage gen html tool is missing.")
    endif(NOT GENHTML_EXE)
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
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND mkdir -p coverage
    COMMAND ${CMAKE_MAKE_PROGRAM} test
    )
if(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "-g -O0 -W -fprofile-arcs -ftest-coverage")
    SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
    add_custom_command(TARGET coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
        COMMENT "Generating coverage files"
        COMMAND ${COV_EXE} -b ${ALL_SOURCES} -o ${CMAKE_BINARY_DIR} | > CoverageSummary.tmp
        COMMAND cat CoverageSummary.tmp
        BYPRODUCTS CoverageSummary.tmp
        COMMAND echo "Coverage files have been output to ${CMAKE_BINARY_DIR}/coverage"
    )
    add_custom_command(TARGET coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
        COMMENT "Generating coverage info"
        COMMAND ${LCOV_EXE} -c -d ${CMAKE_BINARY_DIR}/src/ -o coverage.info
        COMMAND ${LCOV_EXE} --remove coverage.info --output-file coverage.info
          '${CMAKE_CURRENT_SOURCE_DIR}/src/database-server/data-model/test/*'
          '/usr/*'
          '*gtest/*'
        COMMAND echo "Coverage info have been output to ${CMAKE_BINARY_DIR}/coverage/coverage.info"
        BYPRODUCTS coverage.info
        MAIN_DEPENDENCY CoverageSummary.tmp
    )
    add_custom_command(TARGET coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
        COMMENT "Generating html coverage"
        COMMAND ${GENHTML_EXE} --title "Code coverage report for DAQs." coverage.info --output-directory html
        COMMAND echo "Coverage info have been output to ${CMAKE_BINARY_DIR}/coverage/html/index.html"
        COMMAND ${LCOV_EXE} --list coverage.info
        MAIN_DEPENDENCY coverage.info
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
