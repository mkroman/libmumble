# Copyright (c) 2015 Mikkel Kroman, All rights reserved.
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3.0 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library.

# Set the project name
project (libmumble)

# Set minimum required version of CMake
cmake_minimum_required (VERSION 2.8)

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Include the Google Protocol Buffers C Compiler
include (GoogleProtoCC)

# Include source files
include_directories (${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

set (LIBMUMBLE_URL "https://github.com/mkroman/libmumble")
set (LIBMUMBLE_DESCRIPTION "Mumble client library in C.")

set (LIBMUMBLE_LIB_TYPE STATIC CACHE STRING "The library type to build, options are STATIC or SHARED")

option (LIBMUMBLE_AUDIO "Enable audio tramission support (Opus Codec)" TRUE)
option (LIBMUMBLE_LOGGING "Enable logging for debugging purposes" FALSE)
option (LIBMUMBLE_ENABLE_LTO "Enable Link-Time Optimization (requires LLVMgold and gold linker)" FALSE)

if (CMAKE_BUILD_TYPE STREQUAL "")
  set (LOG_LEVEL 0 CACHE STRING "The logging level if logging is enabled")
else ()
  set (LOG_LEVEL 5 CACHE STRING "The logging level if logging is enabled")
endif ()

# Enable logging.
if (LIBMUMBLE_LOGGING)
  # Run cmake with -DLOG_LEVEL=<n> to set the log level.
  if (LOG_LEVEL)
    add_definitions (-DLOG_LEVEL=${LOG_LEVEL})
  endif ()

  add_definitions (-DLIBMUMBLE_LOGGING)
endif ()

# Enable audio transmission and encoding.
if (LIBMUMBLE_AUDIO)
  add_definitions (-DLIBMUMBLE_AUDIO)
endif ()

# Set compiler-specific flags
if (UNIX)
  set (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=gnu99 -Wall -Wextra -pedantic -Werror")

  if (CMAKE_BUILD_TYPE STREQUAL Release)
    if (CMAKE_C_COMPILER_ID STREQUAL Clang AND LIBMUMBLE_ENABLE_LTO)
      set (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-flto")
    endif ()
  endif ()
else ()
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
  include/mumble/channel.h
  include/mumble/user.h
  include/mumble/external.h)

set (client_SOURCES
  src/client.c)

# {{{ Link against OpenSSL
find_package (OpenSSL REQUIRED)

set (libmumble_LIBRARIES ${OPENSSL_LIBRARIES})

include_directories (${OPENSSL_INCLUDE_DIR})
# }}}
# {{{ Link against libev
find_package (LibEV REQUIRED)

set (libmumble_LIBRARIES ${libmumble_LIBRARIES} ${EV_LIBRARIES})
include_directories (${EV_INCLUDE_DIRS})
# }}}
# {{{ Link against the protobuf-c library
find_package (ProtobufC REQUIRED)

include_directories (${PROTOBUF_C_INCLUDE_DIRS})

set (libmumble_LIBRARIES ${libmumble_LIBRARIES} ${PROTOBUF_C_LIBRARIES})
# }}}
# {{{ Generate Google Protocol Buffers
set (PROTO_OUTPUT_DIR "${CMAKE_BINARY_DIR}/proto")
file (MAKE_DIRECTORY ${PROTO_OUTPUT_DIR})

protobuf_c_compile (
  PROTO_FILES
  DESTINATION ${PROTO_OUTPUT_DIR}
  PROTO_PATH "${CMAKE_SOURCE_DIR}/proto"
  TARGETS
    Mumble.proto)

include_directories (${PROTO_OUTPUT_DIR})
# }}}

# Build the library
add_library (mumble ${LIBMUMBLE_LIB_TYPE}
  ${libmumble_SOURCE} 
  ${PROTO_FILES})

target_link_libraries (mumble ${libmumble_LIBRARIES})

# Build the example client
add_executable (client ${client_SOURCES})
target_link_libraries (client mumble)
