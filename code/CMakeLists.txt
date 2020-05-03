cmake_minimum_required(VERSION 3.15)

file(GLOB ACTOR_SOURCES 		"./act/[a-zA-Z]*.cpp")
file(GLOB ACTOR_HEADERS 		"./act/[a-zA-Z]*.hpp")
file(GLOB COMPONENT_SOURCES 	"./cpt/[a-zA-Z]*.cpp")
file(GLOB COMPONENT_HEADERS 	"./cpt/[a-zA-Z]*.hpp")
file(GLOB EVENT_SOURCES 		"./eve/[a-zA-Z]*.cpp")
file(GLOB EVENT_HEADERS 		"./eve/[a-zA-Z]*.hpp")
file(GLOB FIELD_SOURCES 		"./fie/[a-zA-Z]*.cpp")
file(GLOB FIELD_HEADERS 		"./fie/[a-zA-Z]*.hpp")
file(GLOB GRAPHICS_SOURCES 		"./gfx/[a-zA-Z]*.cpp")
file(GLOB GRAPHICS_HEADERS 		"./gfx/[a-zA-Z]*.hpp")
file(GLOB IMGUI_SOURCES 		"./gui/[a-zA-Z]*.cpp")
file(GLOB IMGUI_HEADERS 		"./gui/[a-zA-Z]*.h")
file(GLOB MENU_SOURCES 			"./mnu/[a-zA-Z]*.cpp")
file(GLOB MENU_HEADERS 			"./mnu/[a-zA-Z]*.hpp")
file(GLOB OVERLAY_SOURCES		"./oly/[a-zA-Z]*.cpp")
file(GLOB OVERLAY_HEADERS		"./oly/[a-zA-Z]*.hpp")
file(GLOB PXTONE_SOURCES		"./pxt/[a-zA-Z]*.cpp")
file(GLOB PXTONE_HEADERS		"./pxt/[a-zA-Z]*.h")
file(GLOB RESOURCE_SOURCES		"./res/[a-zA-Z]*.cpp")
file(GLOB RESOURCE_HEADERS		"./res/[a-zA-Z]*.hpp")
file(GLOB SOUND_SOURCES 		"./sfx/[a-zA-Z]*.cpp")
file(GLOB SOUND_HEADERS 		"./sfx/[a-zA-Z]*.hpp")
file(GLOB SYSTEM_SOURCES 		"./sym/[a-zA-Z]*.cpp")
file(GLOB SYSTEM_HEADERS 		"./sym/[a-zA-Z]*.hpp")
file(GLOB UTILITY_SOURCES 		"./utl/[a-zA-Z]*.cpp")
file(GLOB UTILITY_HEADERS 		"./utl/[a-zA-Z]*.hpp")

add_executable(leviathan
	${ACTOR_SOURCES}
	${ACTOR_HEADERS}
	${COMPONENT_SOURCES}
	${COMPONENT_HEADERS}
	${EVENT_SOURCES}
	${EVENT_HEADERS}
	${FIELD_SOURCES}
	${FIELD_HEADERS}
	${GRAPHICS_SOURCES}
	${GRAPHICS_HEADERS}
	${IMGUI_SOURCES}
	${IMGUI_HEADERS}
	${MENU_SOURCES}
	${MENU_HEADERS}
	${OVERLAY_SOURCES}
	${OVERLAY_HEADERS}
	${PXTONE_SOURCES}
	${PXTONE_HEADERS}
	${RESOURCE_SOURCES}
	${RESOURCE_HEADERS}
	${SYSTEM_SOURCES}
	${SYSTEM_HEADERS}
	${SOUND_SOURCES}
	${SOUND_HEADERS}
	${UTILITY_SOURCES}
	${UTILITY_HEADERS}
	"./types.cpp"
	"./types.hpp"
)

if(WIN32)
	target_sources(leviathan PRIVATE "./res/icon.rc")
endif()

if(${CMAKE_VERSION} VERSION_GREATER "3.15")
	target_precompile_headers(leviathan PRIVATE "./stdafx.hpp")
endif()
