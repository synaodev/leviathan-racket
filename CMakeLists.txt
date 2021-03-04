cmake_minimum_required (VERSION 3.13)

# Project definition

project (LEVIATHAN_RACKET)

set (LEVIATHAN_RACKET_VERSION_MAJOR 0)
set (LEVIATHAN_RACKET_VERSION_MINOR 0)
set (LEVIATHAN_RACKET_VERSION_PATCH 0)
set (LEVIATHAN_RACKET_VERSION_TWEAK 0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_C_STANDARD 11)
set (CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_C_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Check for ccache on Linux

if (UNIX)
	find_program (CCACHE_PROGRAM "ccache")
	if (CCACHE_PROGRAM)
		set (C_LAUNCHER ${CCACHE_PROGRAM})
		set (CXX_LAUNCHER ${CCACHE_PROGRAM})
		configure_file ("ccache-c.sh" "launch-c.sh")
		configure_file ("ccache-cxx.sh" "launch-cxx.sh")
		execute_process (COMMAND chmod a+rx
			"${CMAKE_BINARY_DIR}/launch-c.sh"
			"${CMAKE_BINARY_DIR}/launch-cxx.sh"
		)
		if (CMAKE_GENERATOR STREQUAL "Xcode")
			set (CMAKE_XCODE_ATTRIBUTE_CC "${CMAKE_BINARY_DIR}/launch-c.sh")
			set (CMAKE_XCODE_ATTRIBUTE_CXX "${CMAKE_BINARY_DIR}/launch-cxx.sh")
			set (CMAKE_XCODE_ATTRIBUTE_LD "${CMAKE_BINARY_DIR}/launch-c.sh")
			set (CMAKE_XCODE_ATTRIBUTE_LDPLUSPLUS "${CMAKE_BINARY_DIR}/launch-cxx.sh")
		else ()
			set (CMAKE_C_COMPILER_LAUNCHER "${CMAKE_BINARY_DIR}/launch-c.sh")
			set (CMAKE_CXX_COMPILER_LAUNCHER "${CMAKE_BINARY_DIR}/launch-cxx.sh")
		endif ()
	endif ()
endif ()

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

# Building with meta elements

if (NOT DEFINED META_BUILD)
	message ("Defaulting to building without meta menu...")
	set (META_BUILD OFF)
endif ()

# Target

add_executable (lvrk)

# Configure

add_subdirectory ("cmake")

# Source

add_subdirectory ("source")

# Libraries

add_subdirectory ("library")
