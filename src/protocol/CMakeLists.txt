set(PROTO_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/messages.proto)


if(ANDROID)
    find_package(Protobuf 3.4 REQUIRED NO_CMAKE_SYSTEM_PATH
        PATHS ${Protobuf_DIR})
    #deprecated macros
    include(${Protobuf_DIR}/protobuf-module.cmake)
else(ANDROID)
    find_package(Protobuf 3.4 REQUIRED)
endif(ANDROID)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC})
add_library(proto-messages SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(proto-messages SYSTEM PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(proto-messages SYSTEM PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(proto-messages PUBLIC protobuf::libprotobuf)
#protobuf::libprotobuf-lite
