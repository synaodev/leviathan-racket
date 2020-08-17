include (${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

find_path (TMXLITE_INCLUDE_DIR NAMES tmxlite/Config.hpp PATH_SUFFIXES include)

if (VCPKG_TOOLCHAIN)
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		find_library (TMXLITE_LIBRARY NAMES tmxlite-d PATH_SUFFIXES debug/lib)
	else ()
		find_library (TMXLITE_LIBRARY NAMES tmxlite PATH_SUFFIXES lib)
	endif ()
else ()
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		find_library (TMXLITE_LIBRARY NAMES tmxlite-s-d PATH_SUFFIXES lib)
	else ()
		find_library (TMXLITE_LIBRARY NAMES tmxlite-s PATH_SUFFIXES lib)
	endif ()
endif ()

find_package_handle_standard_args (
    TMXLITE
    REQUIRED_VARS TMXLITE_LIBRARY TMXLITE_INCLUDE_DIR
)

