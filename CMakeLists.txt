cmake_minimum_required (VERSION 3.15)

# General settings

project (LEVIATHAN_RACKET)

set (LEVIATHAN_RACKET_VERSION_MAJOR 0)
set (LEVIATHAN_RACKET_VERSION_MINOR 0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_C_STANDARD 11)
set (CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_C_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find packages and libraries

include ("${PROJECT_SOURCE_DIR}/cmake/libraries.cmake")

# Define executable and target compile defitions

add_executable (leviathan)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	target_compile_definitions (leviathan PRIVATE "-DTARGET_VOIDP_EQUALS_64")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
	target_compile_definitions (leviathan PRIVATE "-DTARGET_VOIDP_EQUALS_32")
endif ()

if (VCPKG_TOOLCHAIN)
	target_compile_definitions (leviathan PRIVATE "-DTARGET_TOOLCHAIN_VCPKG")
endif ()

# Platform specific

if (UNIX OR MINGW)
	include ("${PROJECT_SOURCE_DIR}/cmake/platforms/unix.cmake")
elseif (WINDOWS)
	include ("${PROJECT_SOURCE_DIR}/cmake/platforms/win32.cmake")
else ()
	message (FATAL_ERROR "Undefined platform!")
endif ()

# Link/Include

target_link_libraries (leviathan PRIVATE
	Angelscript::angelscript
	${OPENAL_LIBRARY}
	${SDL2_LIBRARIES}
	${TMXLITE_LIBRARY}
	EnTT::EnTT
	glm
)

target_include_directories (leviathan PRIVATE
	${STB_INCLUDE_DIR}
	${OPENAL_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${TMXLITE_INCLUDE_DIR}
)

add_subdirectory("source")
