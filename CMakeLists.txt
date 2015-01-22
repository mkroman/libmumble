project (libmumble)
cmake_minimum_required (VERSION 2.8)

set (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=gnu99 -Wall -Wextra -pedantic -Werror")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set (TARGET_NAME "mumble")

include_directories ("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/src")

# Run cmake with -DLOG_LEVEL=<n> to set the log level.
if (LOG_LEVEL)
  add_definitions (-DLOG_LEVEL=${LOG_LEVEL})
endif ()

set (libmumble_SOURCE
  src/mumble.c
  src/server.c
  src/buffer.c
  src/protocol.c
  src/packets.c
  src/channel.c
  src/user.c
  src/log.c)

set (libmumble_HEADERS
  include/mumble/mumble.h
  include/mumble/server.h
  include/mumble/buffer.h
  include/mumble/protocol.h
  include/mumble/packets.h
  include/mumble/channel.h
  include/mumble/user.h
  include/mumble/log.h)

set (client_SOURCES
  src/client.c)

# {{{ OpenSSL

# Find OpenSSL.
find_package (OpenSSL REQUIRED)

# Link against OpenSSL.
set (${PROJECT_NAME}_LIBS ${OPENSSL_LIBRARIES})

# Include OpenSSL.
include_directories (${OPENSSL_INCLUDE_DIR})

# }}}
# {{{ LibEV

# Find libev.
find_package (LibEV REQUIRED)

# Link against libev.
set (${PROJECT_NAME}_LIBS ${${PROJECT_NAME}_LIBS} "${LIBEV_LIBRARIES}")

# Include libev header directories.
include_directories (${LIBEV_INCLUDE_DIRS})

# }}}
# {{{ Google Protocol Buffers
# Generate protobufs.
include (GoogleProtoCC)

set (PROTO_DIR "${CMAKE_BINARY_DIR}/proto")
file (MAKE_DIRECTORY ${PROTO_DIR})

protobuf_c_compile (
  PROTO_FILES
  DESTINATION ${PROTO_DIR}
  PROTO_PATH "${CMAKE_SOURCE_DIR}/proto"
  TARGETS
    Mumble.proto)

include_directories (${PROTO_DIR})

# Find the protobuf-c library.
find_library (PROTOC_C_LIBRARY
  NAMES protobuf-c
  PATHS ${PROTOBUF_C_ROOT} ${PROTOBUF_C_ROOT}/lib)

include_directories (${PROTOBUF_C_ROOT}/include)
# }}}
set (${PROJECT_NAME}_LIBS ${${PROJECT_NAME}_LIBS} pthread)

add_library (${TARGET_NAME} STATIC
  ${libmumble_SOURCE}
  ${PROTO_FILES})

add_executable (client ${client_SOURCES})

target_link_libraries (${TARGET_NAME} ${PROTOC_C_LIBRARY} ${${PROJECT_NAME}_LIBS})
target_link_libraries (client ${TARGET_NAME})
