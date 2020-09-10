cmake_minimum_required (VERSION 3.15)

# Building with precompiled headers

if (NOT DEFINED STDAFX_BUILD AND NOT MINGW)
	message ("Defaulting to building with precompiled headers...")
	set (STDAFX_BUILD ON)
elseif (${CMAKE_VERSION} VERSION_LESS_EQUAL "3.15" OR MINGW)
	if (STDAFX_BUILD)
		message ("Precompiled headers are unavailable! Sorry!")
	endif ()
	set (STDAFX_BUILD OFF)
endif ()

# Building with ccache

if (WIN32 OR APPLE)
	if (CCACHE_BUILD)
		message ("Cannot use ccache on this system! Sorry!")
	endif ()
	set (CCACHE_BUILD OFF)
elseif (NOT DEFINED CCACHE_BUILD)
	message ("Defaulting to building with ccache, if possible...")
	set (CCACHE_BUILD ON)
endif ()

if (CCACHE_BUILD)
	find_program (CCACHE_PROGRAM "ccache")
	if (CCACHE_PROGRAM)
		set_property (GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
	else ()
		message ("Couldn't find ccache! Sorry!")
	endif ()
endif ()

# Building with meta elements

if (NOT DEFINED META_BUILD)
	message ("Defaulting to building without meta menu...")
	set (META_BUILD OFF)
endif ()

# Building with RTTI (building without is usually not possible)

if (NOT DEFINED RTTI_BUILD)
	message ("Defaulting to building with RTTI...")
	set (RTTI_BUILD ON)
elseif (NOT RTTI_BUILD)
	message ("Warning! Building without RTTI might fail!")
endif ()

# Project definition

project (LeviathanRacket)

set (LeviathanRacket_VERSION_MAJOR 0)
set (LeviathanRacket_VERSION_MINOR 0)
set (LeviathanRacket_VERSION_PATCH 0)
set (LeviathanRacket_VERSION_TWEAK 0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_C_STANDARD 11)
set (CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_C_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Targets

add_executable (naomi)
target_compile_definitions (naomi PRIVATE "-DLEVIATHAN_EXECUTABLE_NAOMI")

add_executable (editor)
target_compile_definitions (editor PRIVATE "-DLEVIATHAN_EXECUTABLE_EDITOR")

# Configure

add_subdirectory ("cmake")

# Source

add_subdirectory ("source")

# Libraries

add_subdirectory ("libraries")
