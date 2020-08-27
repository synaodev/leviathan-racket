cmake_minimum_required (VERSION 3.15)

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

if (NOT DEFINED NAOMI AND NOT DEFINED EDITOR)
	message ("Defaulting to building only \"NAOMI\"...")
	set (NAOMI ON)
	set (EDITOR OFF)
elseif (NOT NAOMI AND NOT EDITOR)
	message (FATAL_ERROR "Error! Neither \"NAOMI\" nor \"EDITOR\" will be built!")
endif ()

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
