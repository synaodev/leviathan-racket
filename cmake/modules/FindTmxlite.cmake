include ("${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake")

find_path (TMXLITE_INCLUDE_DIR NAMES "tmxlite/Config.hpp" PATH_SUFFIXES "include")

if (VCPKG_TOOLCHAIN)
	if (MINGW)
		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			find_library (TMXLITE_LIBRARY NAMES "tmxlite-d.dll" PATH_SUFFIXES "debug/lib" "debug/bin")
		else ()
			find_library (TMXLITE_LIBRARY NAMES "tmxlite.dll" PATH_SUFFIXES "lib" "bin")
		endif ()
	else ()
		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			find_library (TMXLITE_LIBRARY NAMES "tmxlite-s-d" "tmxlite-d" PATH_SUFFIXES "debug/lib" "debug/bin")
		else ()
			find_library (TMXLITE_LIBRARY NAMES "tmxlite-s" "tmxlite" PATH_SUFFIXES "lib" "bin")
		endif ()
	endif ()
else ()
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		find_library (TMXLITE_LIBRARY NAMES "tmxlite-s-d" "tmxlite-d" PATH_SUFFIXES "lib" "bin")
	else ()
		find_library (TMXLITE_LIBRARY NAMES "tmxlite-s" "tmxlite" PATH_SUFFIXES "lib" "bin")
	endif ()
endif ()

find_package_handle_standard_args (
    TMXLITE
    REQUIRED_VARS TMXLITE_LIBRARY TMXLITE_INCLUDE_DIR
)

