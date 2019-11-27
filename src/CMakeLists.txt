cmake_minimum_required(VERSION 3.13)

file(GLOB ACTOR_SOURCES 		"./act/[a-zA-Z]*.cpp")
file(GLOB ACTOR_HEADERS 		"./act/[a-zA-Z]*.hpp")
file(GLOB COMPONENT_SOURCES 	"./cnt/[a-zA-Z]*.cpp")
file(GLOB COMPONENT_HEADERS 	"./cnt/[a-zA-Z]*.hpp")
file(GLOB EVENT_SOURCES 		"./eve/[a-zA-Z]*.cpp")
file(GLOB EVENT_HEADERS 		"./eve/[a-zA-Z]*.hpp")
file(GLOB FIELD_SOURCES 		"./fld/[a-zA-Z]*.cpp")
file(GLOB FIELD_HEADERS 		"./fld/[a-zA-Z]*.hpp")
file(GLOB GRAPHICS_SOURCES 		"./gfx/[a-zA-Z]*.cpp")
file(GLOB GRAPHICS_HEADERS 		"./gfx/[a-zA-Z]*.hpp")
file(GLOB MENU_SOURCES 			"./mnu/[a-zA-Z]*.cpp")
file(GLOB MENU_HEADERS 			"./mnu/[a-zA-Z]*.hpp")
file(GLOB OVERLAY_SOURCES		"./oly/[a-zA-Z]*.cpp")
file(GLOB OVERLAY_HEADERS		"./oly/[a-zA-Z]*.hpp")
file(GLOB PXTONE_SOURCES		"./pxt/[a-zA-Z]*.cpp")
file(GLOB PXTONE_HEADERS		"./pxt/[a-zA-Z]*.h")
file(GLOB SYSTEM_SOURCES 		"./sys/[a-zA-Z]*.cpp")
file(GLOB SYSTEM_HEADERS 		"./sys/[a-zA-Z]*.hpp")
file(GLOB SOUND_SOURCES 		"./sfx/[a-zA-Z]*.cpp")
file(GLOB SOUND_HEADERS 		"./sfx/[a-zA-Z]*.hpp")
file(GLOB UTILITY_SOURCES 		"./utl/[a-zA-Z]*.cpp")
file(GLOB UTILITY_HEADERS 		"./utl/[a-zA-Z]*.hpp")
file(GLOB BASIC_SOURCES			"./[a-zA-Z]*.cpp")
file(GLOB BASIC_HEADERS			"./[a-zA-Z]*.hpp")

add_executable(leviathan
	"${ACTOR_SOURCES}"
	"${ACTOR_HEADERS}"
	"${COMPONENT_SOURCES}"
	"${COMPONENT_HEADERS}"
	"${EVENT_SOURCES}"
	"${EVENT_HEADERS}"
	"${FIELD_SOURCES}"
	"${FIELD_HEADERS}"
	"${GRAPHICS_SOURCES}"
	"${GRAPHICS_HEADERS}"
	"${MENU_SOURCES}"
	"${MENU_HEADERS}"
	"${OVERLAY_SOURCES}"
	"${OVERLAY_HEADERS}"
	"${PXTONE_SOURCES}"
	"${PXTONE_HEADERS}"
	"${SYSTEM_SOURCES}"
	"${SYSTEM_HEADERS}"
	"${SOUND_SOURCES}"
	"${SOUND_HEADERS}"
	"${UTILITY_SOURCES}"
	"${UTILITY_HEADERS}"
	"${BASIC_SOURCES}"
	"${BASIC_HEADERS}"
)

target_include_directories(leviathan PUBLIC
	"${ANGELSCRIPT_INCLUDE}"
	"${ENTT_INCLUDE}"
	"${GLM_INCLUDE}"
	"${NLOHMANN_INCLUDE}"
	"${OPENAL_INCLUDE}"
	"${SDL2_INCLUDE}"
	"${STB_INCLUDE}"
	"${TMXLITE_INCLUDE}"
)

target_link_directories(leviathan PUBLIC
	"${ANGELSCRIPT_LINKDIR}"
	"${OPENAL_LINKDIR}"
	"${SDL2_LINKDIR}"
	"${TMXLITE_LINKDIR}"
)
target_link_libraries(leviathan PUBLIC
	"${ANGELSCRIPT_LIBRARY}"
	"${OPENAL_LIBRARY}"
	"${OPENGL_LIBRARY}"
	"${SDL2_LIBRARY}"
	"${SDL2MAIN_LIBRARY}"
	"${TMXLITE_LIBRARY}"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/W4")
	endif()
	set_target_properties(leviathan PROPERTIES
		CXX_STANDARD 17
		COMPILE_FLAGS "/MP /wd4201 /wd4458 /wd4706 /wd26812"
	)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set_target_properties(leviathan PROPERTIES
		CXX_STANDARD 17
		COMPILE_FLAGS "-MD"
	)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
	set_target_properties(leviathan PROPERTIES
		CXX_STANDARD 17
		COMPILE_FLAGS "-Wno-pragma-pack -Wno-format-security"
	)
else()
	message(FATAL_ERROR "Compiler not supported!")
endif()