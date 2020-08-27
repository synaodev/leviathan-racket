cmake_minimum_required (VERSION 3.15)

# Check for ccache

if (UNIX AND NOT APPLE)
	find_program (CCACHE_PROGRAM "ccache")
	if (CCACHE_PROGRAM)
		message ("Found ccache!")
		set_property (GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
	endif ()
endif ()

# Check arguments

if (NOT DEFINED NAOMI AND NOT DEFINED EDITOR)
	message ("Defaulting to building only \"NAOMI\"...")
	set (NAOMI ON)
	set (EDITOR OFF)
elseif (NOT NAOMI AND NOT EDITOR)
	message (FATAL_ERROR "Error! Neither \"NAOMI\" nor \"EDITOR\" will be built!")
endif ()

# Check if precompiled headers can be used

if (NOT DEFINED STDAFX)
	message ("Defaulting to building without precompiled headers...")
	set (STDAFX OFF)
elseif (${CMAKE_VERSION} VERSION_LESS_EQUAL "3.15" OR MINGW)
	if (STDAFX)
		message ("Precompiled headers are unavailable! Sorry!")
	endif ()
	set (STDAFX OFF)
endif ()

# General settings

project (SYNAODEV_LEVIATHAN)

set (SYNAODEV_LEVIATHAN_VERSION_MAJOR 0)
set (SYNAODEV_LEVIATHAN_VERSION_MINOR 0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_C_STANDARD 11)
set (CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_C_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define targets

if (NAOMI)
	add_executable (naomi)
	target_compile_definitions (naomi PRIVATE "-DLEVIATHAN_EXECUTABLE_NAOMI")
endif ()

if (EDITOR)
	add_executable (editor)
	target_compile_definitions (editor PRIVATE "-DLEVIATHAN_EXECUTABLE_EDITOR")
endif ()

# Configure targets

add_subdirectory ("cmake")

# Source code

add_subdirectory ("source")
