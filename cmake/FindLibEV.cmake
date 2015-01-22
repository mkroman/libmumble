# FindLibEV.cmake
#
# Locates the ev library.
#
# Defines the following variables
#
# EV_FOUND - if the library was found
# EV_LIBRARY - the library path
# EV_INCLUDE_DIR - the library include path

find_path (EV_INCLUDE_DIR ev.h
  HINTS
    ENV EV_DIR
  PATH_SUFFIXES
    ev libev
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /opt/local
    /opt
)

find_library (EV_LIBRARY
  NAMES libev ev
  HINTS
    ENV EV_DIR
  PATH_SUFFIXES
    lib
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /opt/local
    /opt
)

if (EV_LIBRARY)
  # Include the math library on Unix-based machines.
  if (UNIX AND NOT APPLE AND NOT BEOS AND NOT HAIKU)
    find_library (EV_MATH_LIBRARY m)
    set (EV_LIBRARIES "${EV_LIBRARY};${EV_MATH_LIBRARY}" CACHE STRING "LibEV Libraries")
  else ()
    set (EV_LIBRARIES "${LIBEV_LIBRARY}" CACHE STRING "LibEV Libraries")
  endif ()
endif ()

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (EV DEFAULT_MSG
  EV_LIBRARIES EV_INCLUDE_DIR)

mark_as_advanced (EV_INCLUDE_DIR EV_LIBRARY EV_LIBRARIES)
