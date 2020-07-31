cmake_minimum_required (VERSION 3.15)

project (LEVIATHAN_RACKET)

set (LEVIATHAN_RACKET_VERSION_MAJOR 0)
set (LEVIATHAN_RACKET_VERSION_MINOR 0)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable (leviathan)

if (WIN32)
	
	target_compile_definitions (leviathan PRIVATE
		"-DNOMINMAX"
		"-DUNICODE"
		"-D_UNICODE"
		"-DWIN32_LEAN_AND_MEAN"
		"-D_WIN32_WINNT=0x0602"
		"-D_SCL_SECURE_NO_WARNINGS"
		"-D_CRT_SECURE_NO_WARNINGS"
		"-D_CRT_SECURE_NO_DEPRECATE"
		"-D_CRT_NONSTDC_NO_WARNINGS"
	)
	if (MINGW)
		message (FATAL_ERROR "MinGW is not supported!")
	elseif (MSVC)
		# From http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
		foreach (flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE MAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
			string (REPLACE "/W3" "/W4" ${flag_var} "${${flag_var}}")
		endforeach ()
		target_compile_options(leviathan PRIVATE
			"/sdl"	  # Enable security checks
			"/MP"	  # Multi-processor compilation
			"/utf-8"  # UTF-8 source files
			"/WX"	  # Treat linker warnings as errors
			"/wd4201" # nonstandard extension used : nameless struct/union
			"/wd4127" # conditional expression is constant
			"/wd4100" # 'identifier' : unreferenced formal parameter
			"/wd4200" # InputCommon fix temp.
			"/wd4244" # 'conversion' conversion from 'type1' to 'type2', possible loss of data
			"/wd4121" # 'symbol' : alignment of a member was sensitive to packing
			"/wd4324" # Padding was added at the end of a structure because you specified a __declspec(align) value.
			"/wd4714" # function 'function' marked as __forceinline not inlined
			"/wd4351" # new behavior: elements of array 'array' will be default initialized
			"/wd4245" # conversion from 'type1' to 'type2', signed/unsigned mismatch
			"/wd4458" # declaration of 'identifier' hides class member
			"/wd4706" # assignment within conditional expression
			"/wd26812"# Prefer 'enum class' over 'enum' 
			"/w44263" # Non-virtual member function hides base class virtual function
			"/w44265" # Class has virtual functions, but destructor is not virtual
		)
		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			set_target_properties (leviathan PROPERTIES 
				LINK_FLAGS "/SUBSYSTEM:CONSOLE /DEBUG /INCREMENTAL"
				COMPILE_PDB_NAME "leviathan"
				COMPILE_PDB_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
			)
		elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
			set_target_properties(leviathan PROPERTIES 
				LINK_FLAGS "/SUBSYSTEM:WINDOWS /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF"
				COMPILE_PDB_NAME "leviathan"
				COMPILE_PDB_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
			)
		else ()
			set_target_properties(leviathan PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
		endif ()
	endif ()
elseif (UNIX)
	find_package (Threads REQUIRED)
	target_link_libraries (leviathan PRIVATE 
		Threads::Threads
		${CMAKE_DL_LIBS}
	)
	if (NOT APPLE)
		target_link_libraries (leviathan PRIVATE stdc++fs)
	endif ()
endif ()

find_package (SDL2 CONFIG REQUIRED)
find_package (Angelscript CONFIG REQUIRED)
find_package (nlohmann_json CONFIG REQUIRED)
find_package (glm CONFIG REQUIRED)
find_package (EnTT CONFIG REQUIRED)

include (./cmake/FindOpenAL.cmake)
if (NOT OPENAL_FOUND)
	message (FATAL "Could not find OpenAL!")
endif ()

include (./cmake/FindTmxlite.cmake)

target_link_libraries (leviathan PRIVATE
	Angelscript::angelscript
	${OPENAL_LIBRARY}
	SDL2::SDL2
	SDL2::SDL2main
	${TMXLITE_LIBRARY}
	nlohmann_json::nlohmann_json
	EnTT::EnTT
	glm
)

find_path (STB_INCLUDE_DIR "stb.h")

target_include_directories (leviathan PRIVATE 
	${STB_INCLUDE_DIR} 
	${OPENAL_INCLUDE_DIR} 
	${TMXLITE_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
)

add_subdirectory(source)
