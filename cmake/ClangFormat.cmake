find_program(CLANG_FORMAT_EXE clang-format)
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_DAQs_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
    add_custom_target(
      format
      COMMAND ${CLANG_FORMAT_EXE} -i -style=Google ${ALL_DAQs_SOURCES}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Formating source code")
    find_package(Git)
    if(Git_FOUND)
        add_custom_target(
            check-format
            DEPENDS format
            COMMAND ${GIT_EXECUTABLE} diff --exit-code
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Checking format")
    else(Git_FOUND)
        message(WARNING "Git not found.")
    endif(Git_FOUND)
else(CLANG_FORMAT_EXE)
    message(WARNING "Clang format not found.")
endif(CLANG_FORMAT_EXE)
