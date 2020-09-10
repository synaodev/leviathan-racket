cmake_minimum_required (VERSION 3.15)

if (VCPKG_TOOLCHAIN)
	target_compile_definitions (naomi PRIVATE "-DLEVIATHAN_USES_VCPKG")
endif ()

if (META_BUILD)
	target_compile_definitions (naomi PRIVATE "-DLEVIATHAN_USES_META")
endif ()

if (STDAFX_BUILD)
	target_precompile_headers (naomi PRIVATE "stdafx.hpp")
	target_precompile_headers (editor PRIVATE "stdafx.hpp")
endif ()

configure_file ("version.inc" "leviathan_racket.hpp" @ONLY)

target_compile_definitions (naomi PRIVATE "-DLEVIATHAN_DEFINES_VERSION")
target_compile_definitions (editor PRIVATE "-DLEVIATHAN_DEFINES_VERSION")
target_include_directories (naomi PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories (editor PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory("actor")
add_subdirectory("audio")
add_subdirectory("component")
add_subdirectory("field")
add_subdirectory("menu")
add_subdirectory("overlay")
add_subdirectory("resource")
add_subdirectory("system")
add_subdirectory("utility")
add_subdirectory("video")
