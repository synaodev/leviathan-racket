# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Tmxlite::tmxlite" for configuration "Release"

# Windows
if(WIN32)
  set_property(TARGET Tmxlite::tmxlite APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
  set_target_properties(Tmxlite::tmxlite PROPERTIES
    IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/tmxlite.lib"
    IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/tmxlite.dll"
  )
  list(APPEND _IMPORT_CHECK_TARGETS Tmxlite::tmxlite )
  list(APPEND _IMPORT_CHECK_FILES_FOR_Tmxlite::tmxlite "${_IMPORT_PREFIX}/lib/tmxlite.lib" "${_IMPORT_PREFIX}/bin/tmxlite.dll" )
# MacOS
elseif(APPLE)
  set_property(TARGET Tmxlite::tmxlite APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
  set_target_properties(Tmxlite::tmxlite PROPERTIES
    IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtmxlite.dylib"
  )
  list(APPEND _IMPORT_CHECK_TARGETS Tmxlite::tmxlite )
  list(APPEND _IMPORT_CHECK_FILES_FOR_Tmxlite::tmxlite "${_IMPORT_PREFIX}/lib/libtmxlite.dylib" )
# Unix
else()
  set_property(TARGET Tmxlite::tmxlite APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
  set_target_properties(Tmxlite::tmxlite PROPERTIES
    IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtmxlite.so"
  )
  list(APPEND _IMPORT_CHECK_TARGETS Tmxlite::tmxlite )
  list(APPEND _IMPORT_CHECK_FILES_FOR_Tmxlite::tmxlite "${_IMPORT_PREFIX}/lib/libtmxlite.so" )
endif()

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
