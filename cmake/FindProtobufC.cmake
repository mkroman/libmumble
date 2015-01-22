# FindProtobufC.cmake
#
# Locates the protobuf-c library.
#
# Defines the following variables
#
# PROTOBUF_C_FOUND - if the library was found
# PROTOBUF_C_LIBRARY - the library path
# PROTOBUF_C_INCLUDE_DIR - the include path

find_path (PROTOBUF_C_INCLUDE_DIR protobuf-c.h
  HINTS
    ENV PROTOBUF_C_DIR
  PATH_SUFFIXES protobuf-c
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /opt/local
    /opt
)

find_library (PROTOBUF_C_LIBRARY
  NAMES protobuf-c
  HINTS
    ENV PROTOBUF_C_DIR
  PATH_SUFFIXES lib protobuf-c
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /opt/local
    /opt
)

if (PROTOBUF_C_LIBRARY)
  set (PROTOBUF_C_LIBRARIES "${PROTOBUF_C_LIBRARY}" CACHE STRING "Protobuf C Libraries")
endif ()

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (PROTOBUF_C DEFAULT_MSG
  PROTOBUF_C_LIBRARIES PROTOBUF_C_INCLUDE_DIR)

mark_as_advanced (PROTOBUF_C_INCLUDE_DIR PROTOBUF_C_LIBRARY)
