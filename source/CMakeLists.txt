cmake_minimum_required (VERSION 3.15)

target_sources (leviathan PRIVATE
	"types.cpp" "types.hpp"
)

if (${CMAKE_VERSION} VERSION_GREATER "3.15")
	target_precompile_headers (leviathan PRIVATE "stdafx.hpp")
endif ()

add_subdirectory("actor")
add_subdirectory("audio")
add_subdirectory("component")
add_subdirectory("event")
add_subdirectory("field")
add_subdirectory("imgui")
add_subdirectory("menu")
add_subdirectory("overlay")
add_subdirectory("pxtone")
add_subdirectory("resource")
add_subdirectory("system")
add_subdirectory("utility")
add_subdirectory("video")
