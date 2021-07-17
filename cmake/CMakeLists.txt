cmake_minimum_required (VERSION 3.13)

# Platform

if (WIN32 AND NOT MINGW)
	include ("${PROJECT_SOURCE_DIR}/cmake/platforms/win32.cmake")
elseif (UNIX)
	include ("${PROJECT_SOURCE_DIR}/cmake/platforms/unix.cmake")
else ()
	message (FATAL_ERROR "Unsupported platform!")
endif ()

# Find Packages/Libraries

set (GENERAL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

find_package (Angelscript CONFIG REQUIRED)
find_package (EnTT CONFIG REQUIRED)
find_package (nlohmann_json CONFIG REQUIRED)
find_package (fmt CONFIG REQUIRED)

include ("${PROJECT_SOURCE_DIR}/cmake/modules/FindGLM.cmake")
if (NOT GLM_FOUND)
	message (FATAL_ERROR "Could not find glm!")
endif ()

if (VCPKG_TOOLCHAIN)
	find_package (OpenAL CONFIG REQUIRED)
	set (OPENAL_LIBRARY OpenAL::OpenAL)
	find_package (SDL2 CONFIG REQUIRED)
	if (APPLE)
		set (SDL2_LIBRARIES SDL2::SDL2)
	else ()
		set (SDL2_LIBRARIES SDL2::SDL2main SDL2::SDL2)
	endif ()
else ()
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

# Link/Include

target_link_libraries (lvrk PRIVATE
	Angelscript::angelscript
	EnTT::EnTT
	nlohmann_json::nlohmann_json
	fmt::fmt
	${OPENAL_LIBRARY}
	${SDL2_LIBRARIES}
)
target_include_directories (lvrk PRIVATE
	${GENERAL_INCLUDE_DIR}
	${OPENAL_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
)
