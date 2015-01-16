# Find the protoc-c executable.
find_program (PROTOBUF_C_PROTOC_EXECUTABLE
  NAMES protoc-c
  DOC "The Google Protocol Buffers C Compiler")

if (NOT PROTOBUF_C_PROTOC_EXECUTABLE)
  message (FATAL_ERROR "Could not find the Google Protobuf C Compiler!")
endif()

# Usage:
# protobuf_compile([variable] [DESTINATION <destination>] [PROTO_PATH <proto path>] [TARGETS <file> <filen> ...])
function (PROTOBUF_C_COMPILE)
  set (oneValueArgs DESTINATION PROTO_PATH)
  set (multiValueArgs TARGETS)

  cmake_parse_arguments(PROTOBUF_COMPILE "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT IS_DIRECTORY ${PROTOBUF_COMPILE_DESTINATION})
    message (FATAL_ERROR "Compile destination folder does not exist!")
  endif()

  foreach (proto_file IN LISTS PROTOBUF_COMPILE_TARGETS)
    get_filename_component (PROTO_NAME ${proto_file} NAME_WE)
    list (APPEND PROTOBUF_COMPILE_OUTPUT_PATHS
      "${PROTOBUF_COMPILE_DESTINATION}/${PROTO_NAME}.pb-c.h"
      "${PROTOBUF_COMPILE_DESTINATION}/${PROTO_NAME}.pb-c.c")
    list (APPEND TARGET_PATHS "${PROTOBUF_COMPILE_PROTO_PATH}/${proto_file}")
  endforeach()

  if (PROTOBUF_COMPILE_UNPARSED_ARGUMENTS)
    set (${PROTOBUF_COMPILE_UNPARSED_ARGUMENTS} ${PROTOBUF_COMPILE_OUTPUT_PATHS} PARENT_SCOPE)
  endif()

  add_custom_command (
    OUTPUT ${PROTOBUF_COMPILE_OUTPUT_PATHS}
    COMMAND ${PROTOBUF_C_PROTOC_EXECUTABLE}
    ARGS
      --proto_path ${PROTOBUF_COMPILE_PROTO_PATH} --c_out ${PROTOBUF_COMPILE_DESTINATION}
      ${TARGET_PATHS}
    DEPENDS
      ${PROTOBUF_C_PROTOC_EXECUTABLE}
    COMMENT "Generating Protocol Buffers"
    VERBATIM)

  set_source_files_properties (
    ${PROTOBUF_COMPILE_OUTPUT_PATHS}
    PROPERTIES
      GENERATED TRUE)
endfunction()

