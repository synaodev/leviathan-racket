cmake_minimum_required(VERSION 3.13)

# ------------------------------
# ANGELSCRIPT_INCLUDE
# ENTT_INCLUDE
# GLM_INCLUDE
# NLOHMANN_INCLUDE
# OPENAL_INCLUDE
# SDL2_INCLUDE
# STB_INCLUDE
# TMXLITE_INCLUDE
# ------------------------------
# ANGELSCRIPT_LINKDIR
# OPENAL_LINKDIR
# SDL2_LINKDIR
# TMXLITE_LINKDIR
# ------------------------------
# ANGELSCRIPT_LIBRARY
# OPENAL_LIBRARY
# OPENGL_LIBRARY
# SDL2_LIBRARY
# SDL2MAIN_LIBRARY
# TMXLITE_LIBRARY
# ------------------------------

if(WIN32) # Windows 32/64
	set(ANGELSCRIPT_INCLUDE "C:/Libs/angelscript/angelscript/include")
	set(ANGELSCRIPT_LINKDIR "C:/Libs/angelscript/angelscript/lib")
	set(ENTT_INCLUDE 		"C:/Libs/entt/src")
	set(GLM_INCLUDE			"C:/Libs/glm")
	set(NLOHMANN_INCLUDE	"C:/Libs/nlohmann")
	set(OPENAL_INCLUDE		"C:/Libs/openal/include")
	set(OPENAL_LIBRARY 		"openal32.lib")
	set(OPENGL_LIBRARY		"opengl32.lib")
	set(SDL2_INCLUDE		"C:/Libs/sdl2/include")
	set(SDL2_LIBRARY 		"SDL2.lib")
	set(SDL2MAIN_LIBRARY	"SDL2main.lib")
	set(STB_INCLUDE			"C:/Libs/stb")
	set(TMXLITE_INCLUDE		"C:/Libs/tmxlite/tmxlite/include")
	if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
		set(OPENAL_LINKDIR 	"C:/Libs/openal/lib/x64")
		set(SDL2_LINKDIR 	"C:/Libs/sdl2/lib/x64")
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(ANGELSCRIPT_LIBRARY "angelscript64d.lib")
			set(TMXLITE_LINKDIR 	"C:/Libs/tmxlite/tmxlite/x64/bin/DebugShared")
			set(TMXLITE_LIBRARY 	"libtmxlite-d.lib")
		else()
			set(ANGELSCRIPT_LIBRARY "angelscript64.lib")
			set(TMXLITE_LINKDIR 	"C:/Libs/tmxlite/tmxlite/x64/bin/ReleaseShared")
			set(TMXLITE_LIBRARY 	"libtmxlite.lib")
		endif()
	else()
		set(OPENAL_LINKDIR 	"C:/Libs/openal/lib/x86")
		set(SDL2_LINKDIR 	"C:/Libs/sdl2/lib/x86")
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(ANGELSCRIPT_LIBRARY "angelscriptd.lib")
			set(TMXLITE_LINKDIR 	"C:/Libs/tmxlite/tmxlite/bin/DebugShared")
			set(TMXLITE_LIBRARY 	"libtmxlite-d.lib")
		else()
			set(ANGELSCRIPT_LIBRARY "angelscript.lib")
			set(TMXLITE_LINKDIR 	"C:/Libs/tmxlite/tmxlite/bin/ReleaseShared")
			set(TMXLITE_LIBRARY 	"libtmxlite.lib")
		endif()
	endif()
elseif(UNIX) # Linux and BSD
	message(FATAL_ERROR "Platform not supported!")
elseif(APPLE) # MacOS
	message(FATAL_ERROR "Platform not supported!")
else()
	message(FATAL_ERROR "Platform not supported!")
endif()

project(Leviathan)
add_subdirectory(src)