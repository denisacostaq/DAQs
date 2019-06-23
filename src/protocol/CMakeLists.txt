find_package(Protobuf 3.6.1 REQUIRED)
set(PROTO_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/messages.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC})
add_library(proto-messages ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(proto-messages SYSTEM PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(proto-messages SYSTEM PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(proto-messages PUBLIC ${Protobuf_LIBRARIES})
