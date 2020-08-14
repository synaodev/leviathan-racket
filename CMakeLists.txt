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

# Find most packages & libaries

find_package (Angelscript CONFIG REQUIRED)
find_package (glm CONFIG REQUIRED)
find_package (EnTT CONFIG REQUIRED)

include ("${PROJECT_SOURCE_DIR}/cmake/modules/FindTmxlite.cmake")
if (NOT TMXLITE_FOUND)
	message (FATAL_ERROR "Could not find Tmxlite!")
endif ()

include ("${PROJECT_SOURCE_DIR}/cmake/modules/FindPugiXML.cmake")
if (NOT PUGIXML_FOUND)
	message (FATAL_ERROR "Could not find PugiXML!")
endif ()

include ("${PROJECT_SOURCE_DIR}/cmake/modules/FindNlohmannJson.cmake")
if (NOT NLOHMANN_JSON_FOUND)
	message (FATAL_ERROR "Could not find JSON library!")
endif ()

include ("${PROJECT_SOURCE_DIR}/cmake/modules/FindStb.cmake")
if (NOT STB_FOUND)
	message (FATAL_ERROR "Could not find stb!")
endif ()

# Define executable

add_executable (leviathan)

# Platform specific

include ("${PROJECT_SOURCE_DIR}/cmake/platforms/libs.cmake")

if (WIN32)
	include ("${PROJECT_SOURCE_DIR}/cmake/platforms/win32.cmake")
elseif (UNIX)
	include ("${PROJECT_SOURCE_DIR}/cmake/platforms/unix.cmake")
else ()
	message (FATAL_ERROR "Undefined platform!")
endif ()

# Link/Include

target_link_libraries (leviathan PRIVATE
	Angelscript::angelscript
	${OPENAL_LIBRARY}
	${SDL2_LIBRARIES}
	${TMXLITE_LIBRARY}
	${PUGIXML_LIBRARY}
	EnTT::EnTT
	glm
)

target_include_directories (leviathan PRIVATE
	${STB_INCLUDE_DIR}
	${OPENAL_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${TMXLITE_INCLUDE_DIR}
	${PUGIXML_INCLUDE_DIR}
)

add_subdirectory("source")
