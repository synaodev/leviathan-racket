# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Tmxlite::tmxlite" for configuration "Debug"

# Windows
if(WIN32)
  set_property(TARGET Tmxlite::tmxlite APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
  set_target_properties(Tmxlite::tmxlite PROPERTIES
    IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/tmxlite-d.lib"
    IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/tmxlite-d.dll"
  )
  list(APPEND _IMPORT_CHECK_TARGETS Tmxlite::tmxlite )
  list(APPEND _IMPORT_CHECK_FILES_FOR_Tmxlite::tmxlite "${_IMPORT_PREFIX}/debug/lib/tmxlite-d.lib" "${_IMPORT_PREFIX}/debug/bin/tmxlite-d.dll" )
# MacOS
elseif(APPLE)
  set_property(TARGET Tmxlite::tmxlite APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
  set_target_properties(Tmxlite::tmxlite PROPERTIES
    IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/libtmxlite-d.dylib"
  )
  list(APPEND _IMPORT_CHECK_TARGETS Tmxlite::tmxlite )
  list(APPEND _IMPORT_CHECK_FILES_FOR_Tmxlite::tmxlite "${_IMPORT_PREFIX}/debug/lib/libtmxlite-d.dylib" )
# Unix
else()
  set_property(TARGET Tmxlite::tmxlite APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
  set_target_properties(Tmxlite::tmxlite PROPERTIES
    IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/libtmxlite-d.so"
  )
  list(APPEND _IMPORT_CHECK_TARGETS Tmxlite::tmxlite )
  list(APPEND _IMPORT_CHECK_FILES_FOR_Tmxlite::tmxlite "${_IMPORT_PREFIX}/debug/lib/libtmxlite-d.so" )
endif()

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
