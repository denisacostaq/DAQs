#TODO(denisacostaq@gmail.com): Use project version.
set(DOXYGEN_PROJECT_NUMBER 1.0.0)
set(DOXYGEN_MSCFILE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../database-server/doc)
doxygen_add_docs(doc
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ALL
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating protocol documentation...")
