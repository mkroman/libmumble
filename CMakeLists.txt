project (libmumble)
cmake_minimum_required (VERSION 2.8)

set (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=gnu99")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include_directories (include/)

if (WIN32)
  add_definitions (-DWIN32 -D__WINDOWS__ -DUNICODE)
else ()
  add_definitions (-DUNIX)
endif ()

set (libmumble_SOURCE
  src/mumble.c
  src/server.c
)

set (libmumble_HEADERS
  include/mumble.h
  include/server.h)

set (client_SOURCES
  src/client.c)

# Find OpenSSL.
find_package (OpenSSL REQUIRED)

# Link against OpenSSL.
set (${PROJECT_NAME}_LIBS ${OPENSSL_SSL_LIBRARY})

# Include OpenSSL.
include_directories (${OPENSSL_INCLUDE_DIR})

# Find libev.
find_package (LibEV REQUIRED)

# Link against libev.
set (${PROJECT_NAME}_LIBS ${${PROJECT_NAME}_LIBS} "${LIBEV_LIBRARIES}")

# Include libev header directories.
include_directories (${LIBEV_INCLUDE_DIRS})

# Find the protoc-c executable.
find_program (PROTOBUF_C_PROTOC_EXECUTABLE
  NAMES protoc-c
  DOC "The Google Protocol Buffers C Compiler")

# Generate protobufs.
add_custom_command (
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Mumble.pb-c.c" "${CMAKE_CURRENT_BINARY_DIR}/Mumble.pb-c.h"
  COMMAND ${PROTOBUF_C_PROTOC_EXECUTABLE}
  ARGS --proto_path ${CMAKE_CURRENT_SOURCE_DIR}/proto --c_out ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/Mumble.proto
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/proto/Mumble.proto ${PROTOBUF_C_PROTOC_EXECUTABLE}
  COMMENT "Generating protobufs"
  VERBATIM)

set_source_files_properties (
  ${CMAKE_CURRENT_BINARY_DIR}/Mumble.pb-c.c
  ${CMAKE_CURRENT_BINARY_DIR}/Mumble.pb-c.h
  PROPERTIES
    GENERATED TRUE)

# Find the protobuf-c library.
find_library (PROTOC_C_LIBRARY
  NAMES protobuf-c
  PATHS ${PROTOBUF_C_ROOT} ${PROTOBUF_C_ROOT}/lib)

include_directories (${PROTOBUF_C_ROOT}/include)

add_library (${PROJECT_NAME} STATIC
  ${libmumble_SOURCE} ${libmumble_HEADERS}
  ${CMAKE_CURRENT_BINARY_DIR}/Mumble.pb-c.c
  ${CMAKE_CURRENT_BINARY_DIR}/Mumble.pb-c.h)

add_executable (client ${client_SOURCES})

target_link_libraries (${PROJECT_NAME} ${PROTOC_C_LIBRARY} ${${PROJECT_NAME}_LIBS})
target_link_libraries (client ${PROJECT_NAME})