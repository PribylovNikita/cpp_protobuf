set(BINARY ${CMAKE_PROJECT_NAME}_test)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(SOURCE_DIR)

set(TEST_SOURCES
        Test_DelimitedMessagesStreamParser.cpp
        Test_Helpers.cpp
        main.cpp)

include(FindProtobuf)
find_package(GTest REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIR})
set(ProtobufIncludePath ${CMAKE_BINARY_DIR}
        CACHE INTERNAL "Path to generated protobuf files.")
include_directories(${ProtobufIncludePath})

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${CMAKE_SOURCE_DIR}/messages.proto)
add_library(messages
        ${PROTO_HEADER}
        ${PROTO_SRC})

add_executable(${BINARY} ${TEST_SOURCES})
target_include_directories(${BINARY} PRIVATE ${SOURCE_DIR}/protobuf_parser)
target_link_libraries(${BINARY} messages ${Protobuf_LIBRARIES} GTest::GTest)