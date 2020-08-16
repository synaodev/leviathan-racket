cmake_minimum_required (VERSION 3.15)

# Set pointer size

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	target_compile_definitions (leviathan PRIVATE "-DTARGET_VOIDP_EQUALS_64")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
	target_compile_definitions (leviathan PRIVATE "-DTARGET_VOIDP_EQUALS_32")
else ()
	message (FATAL_ERROR "Undefined pointer size!")
endif ()

# Set ImGui OpenGL Loader to GLAD

target_compile_definitions (leviathan PRIVATE "-DIMGUI_IMPL_OPENGL_LOADER_GLAD")

# Load SDL2 and OpenAL here since they can be finnicky

if (VCPKG_TOOLCHAIN)
	target_compile_definitions (leviathan PRIVATE "-DTARGET_TOOLCHAIN_VCPKG")

	find_package (OpenAL CONFIG REQUIRED)
	set (OPENAL_LIBRARY OpenAL::OpenAL)

	find_package (SDL2 CONFIG REQUIRED)
	if (APPLE)
		set (SDL2_LIBRARIES SDL2::SDL2)
	else ()
		set (SDL2_LIBRARIES SDL2::SDL2 SDL2::SDL2main)
	endif ()
else ()
	target_compile_definitions (leviathan PRIVATE "-DTARGET_TOOLCHAIN_DEFAULT")

	include ("${PROJECT_SOURCE_DIR}/cmake/modules/FindOpenAL.cmake")
	if (NOT OPENAL_FOUND)
		message (FATAL_ERROR "Could not find OpenAL!")
	endif ()
	include ("${PROJECT_SOURCE_DIR}/cmake/modules/FindSDL2.cmake")
	if (NOT SDL2_FOUND)
		message (FATAL_ERROR "Could not find SDL2!")
	elseif (NOT SDL2MAIN_FOUND AND NOT APPLE)
		message (FATAL_ERROR "Could not find SDL2-Main!")
	endif ()
endif ()
