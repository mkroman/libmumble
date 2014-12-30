# FindLibEV.cmake
#
# Locates libev.
#
# Defines the variables:
#
# LIBEV_INCLUDE_DIR
# LIBEV_LIBRARY

find_path (LIBEV_INCLUDE_DIR ev.h
  HINTS
    ENV EV_DIR
  PATH_SUFFIXES
    ev libev
)

find_library (LIBEV_LIBRARY
  NAMES libev ev
  HINTS
    ENV EV_DIR
  PATH_SUFFIXES
    lib
)

if (LIBEV_LIBRARY)
  set (LIBEV_LIBRARIES "${LIBEV_LIBRARY}")
endif ()

if (EXISTS "${LIBEV_INCLUDE_DIR}/ev.h")
  set (LIBEV_INCLUDE_DIRS "${LIBEV_INCLUDE_DIR}")
endif ()

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (libev DEFAULT_MSG
  LIBEV_LIBRARY LIBEV_INCLUDE_DIR)

mark_as_advanced (LIBEV_INCLUDE_DIR LIBEV_INCLUDE_DIRS LIBEV_LIBRARY
  LIBEV_LIBRARIES)
