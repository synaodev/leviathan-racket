cmake_minimum_required(VERSION 3.15)

set(CMAKE_USER_MAKE_RULES_OVERRIDE "./Options.cmake")

project(Leviathan)
add_subdirectory(src)

find_package(Angelscript CONFIG REQUIRED)
find_path(Angelscript_INCLUDE "angelscript.h")

find_package(OpenAL CONFIG REQUIRED)
find_path(OpenAL_INCLUDE "AL/al.h")

find_package(OpenGL REQUIRED)
find_path(OpenGL_INCLUDE "gl/gl.h")

find_package(SDL2 CONFIG REQUIRED)
find_path(SDL2_INCLUDE "SDL2/SDL.h")

find_package(Tmxlite CONFIG REQUIRED)
find_path(Tmxlite_INCLUDE "tmxlite/Config.hpp")

# find_package(EnTT REQUIRED)
find_path(EnTT_INCLUDE "entt/entt.hpp")

# find_package(GLM REQUIRED)
find_path(GLM_INCLUDE "glm/glm.hpp")

target_include_directories(leviathan PRIVATE
	${Angelscript_INCLUDE}
	${OpenAL_INCLUDE}
	# ${OpenGL_INCLUDE}
	${SDL2_INCLUDE}
	${Tmxlite_INCLUDE}
	${EnTT_INCLUDE}
	${GLM_INCLUDE}
)

target_link_libraries(leviathan PRIVATE
	Angelscript::angelscript
	OpenAL::OpenAL
	SDL2::SDL2
	SDL2::SDL2main
	Tmxlite::tmxlite
)

if(NOT WIN32)
	find_package(Threads REQUIRED)
	target_link_libraries(leviathan PRIVATE 
		OpenGL::OpenGL
		Threads::Threads
	)
endif()

if(MSVC)
	set_target_properties(leviathan PROPERTIES
		CXX_STANDARD 17
		COMPILE_FLAGS "/MP /sdl /wd4201 /wd4458 /wd4706 /wd26812"
		COMPILE_PDB_NAME leviathan
		COMPILE_PDB_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
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
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set_target_properties(leviathan PROPERTIES CXX_STANDARD 17)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set_target_properties(leviathan PROPERTIES CXX_STANDARD 17)
else()
	message(FATAL_ERROR "Compiler not supported!")
endif()
