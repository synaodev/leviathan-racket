cmake_minimum_required(VERSION 3.13)

# ------------------------------
# ANGELSCRIPT_INCLUDE
# ENTT_INCLUDE
# GLM_INCLUDE
# OPENAL_INCLUDE
# SDL2_INCLUDE
# TMXLITE_INCLUDE
# ------------------------------
# ANGELSCRIPT_LINKDIR
# OPENAL_LINKDIR
# SDL2_LINKDIR
# TMXLITE_LINKDIR
# ------------------------------
# ANGELSCRIPT_LIBRARY
# OPENAL_LIBRARY"
# OPENGL_LIBRARY
# SDL2_LIBRARY
# SDL2_MAIN_LIBRARY
# TMXLITE_LIBRARY
# ------------------------------

project(Leviathan)
add_subdirectory(src)

# ------------------------------ Windows
if(WIN32)
	target_include_directories(leviathan PUBLIC
		"C:/Libs/angelscript/angelscript/include"
		"C:/Libs/entt/src"
		"C:/Libs/glm"
		"C:/Libs/openal/include"
		"C:/Libs/sdl2/include"
		"C:/Libs/tmxlite/tmxlite/include"
	)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(ANGELSCRIPT_LIBRARY "angelscript64d.lib")
		set(TMXLITE_LIBRARY 	"libtmxlite-d.lib")
		target_link_directories(leviathan PUBLIC
			"C:/Libs/angelscript/angelscript/lib"
			"C:/Libs/openal/lib/x64"
			"C:/Libs/sdl2/lib/x64"
			"C:/Libs/tmxlite/tmxlite/x64/bin/DebugShared"
		)
	else()
		set(ANGELSCRIPT_LIBRARY "angelscript64.lib")
		set(TMXLITE_LIBRARY 	"libtmxlite.lib")
		target_link_directories(leviathan PUBLIC
			"C:/Libs/angelscript/angelscript/lib"
			"C:/Libs/openal/lib/x64"
			"C:/Libs/sdl2/lib/x64"
			"C:/Libs/tmxlite/tmxlite/x64/bin/ReleaseShared"
		)
	endif()
	set(OPENAL_LIBRARY 		"openal32.lib")
	set(OPENGL_LIBRARY 		"opengl32.lib")
	set(SDL2_LIBRARY 		"SDL2.lib")
	set(SDL2_MAIN_LIBRARY 	"SDL2main.lib")
# ------------------------------ MacOS
elseif(APPLE)
	message(FATAL_ERROR "Platform not supported!")
# ------------------------------ Linux
elseif(UNIX)
	message(FATAL_ERROR "Platform not supported!")
else()
	message(FATAL_ERROR "Platform not supported!")
endif()

target_link_libraries(leviathan PUBLIC
	"${ANGELSCRIPT_LIBRARY}"
	"${OPENAL_LIBRARY}"
	"${OPENGL_LIBRARY}"
	"${SDL2_LIBRARY}"
	"${SDL2_MAIN_LIBRARY}"
	"${TMXLITE_LIBRARY}"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/W4")
	endif()
	if(CMAKE_CXX_FLAGS MATCHES "/Zi")
		string(REGEX REPLACE "/Zi" "/ZI" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/ZI")
	endif()
	set_target_properties(leviathan PROPERTIES
		CXX_STANDARD 17
		COMPILE_FLAGS "/MP /sdl /wd4201 /wd4458 /wd4706 /wd26812"
		COMPILE_PDB_NAME "leviathan"
		COMPILE_PDB_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}"
	)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set_target_properties(leviathan PROPERTIES
			LINK_FLAGS "/SUBSYSTEM:CONSOLE /DEBUG /INCREMENTAL"
		)
	else()
		set_target_properties(leviathan PROPERTIES
			LINK_FLAGS "/SUBSYSTEM:WINDOWS /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF"
		)
	endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set_target_properties(leviathan PROPERTIES CXX_STANDARD 17)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
	set_target_properties(leviathan PROPERTIES CXX_STANDARD 17)
else()
	message(FATAL_ERROR "Compiler not supported!")
endif()
