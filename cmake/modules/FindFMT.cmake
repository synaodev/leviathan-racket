include (${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

find_path (FMT_INCLUDE_DIR NAMES "fmt/format.h" PATH_SUFFIXES "include")

if (VCPKG_TOOLCHAIN)
	if (MINGW)
		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			find_library (FMT_LIBRARY NAMES "fmtd.dll" PATH_SUFFIXES "debug/lib")
		else ()
			find_library (FMT_LIBRARY NAMES "fmt.dll" PATH_SUFFIXES "lib")
		endif ()
	else ()
		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			find_library (FMT_LIBRARY NAMES "fmtd" PATH_SUFFIXES "debug/lib")
		else ()
			find_library (FMT_LIBRARY NAMES "fmt" PATH_SUFFIXES "lib")
		endif ()
	endif ()
else ()
	find_library (FMT_LIBRARY NAMES "fmt" PATH_SUFFIXES "lib")
endif ()

find_package_handle_standard_args (
    FMT
    REQUIRED_VARS FMT_LIBRARY FMT_INCLUDE_DIR
)
