cmake_minimum_required(VERSION 3.15)

file(GLOB ACTOR_SOURCES 		"./actor/[a-zA-Z]*.cpp")
file(GLOB ACTOR_HEADERS 		"./actor/[a-zA-Z]*.hpp")
file(GLOB AUDIO_SOURCES 		"./audio/[a-zA-Z]*.cpp")
file(GLOB AUDIO_HEADERS 		"./audio/[a-zA-Z]*.hpp")
file(GLOB COMPONENT_SOURCES 	"./component/[a-zA-Z]*.cpp")
file(GLOB COMPONENT_HEADERS 	"./component/[a-zA-Z]*.hpp")
file(GLOB EVENT_SOURCES 		"./event/[a-zA-Z]*.cpp")
file(GLOB EVENT_HEADERS 		"./event/[a-zA-Z]*.hpp")
file(GLOB FIELD_SOURCES 		"./field/[a-zA-Z]*.cpp")
file(GLOB FIELD_HEADERS 		"./field/[a-zA-Z]*.hpp")
file(GLOB IMGUI_SOURCES 		"./imgui/[a-zA-Z]*.cpp")
file(GLOB IMGUI_HEADERS 		"./imgui/[a-zA-Z]*.h")
file(GLOB MENU_SOURCES 			"./menu/[a-zA-Z]*.cpp")
file(GLOB MENU_HEADERS 			"./menu/[a-zA-Z]*.hpp")
file(GLOB OVERLAY_SOURCES		"./overlay/[a-zA-Z]*.cpp")
file(GLOB OVERLAY_HEADERS		"./overlay/[a-zA-Z]*.hpp")
file(GLOB PXTONE_SOURCES		"./pxtone/[a-zA-Z]*.cpp")
file(GLOB PXTONE_HEADERS		"./pxtone/[a-zA-Z]*.h")
file(GLOB RESOURCE_SOURCES		"./resource/[a-zA-Z]*.cpp")
file(GLOB RESOURCE_HEADERS		"./resource/[a-zA-Z]*.hpp")
file(GLOB SYSTEM_SOURCES 		"./system/[a-zA-Z]*.cpp")
file(GLOB SYSTEM_HEADERS 		"./system/[a-zA-Z]*.hpp")
file(GLOB UTILITY_SOURCES 		"./utility/[a-zA-Z]*.cpp")
file(GLOB UTILITY_HEADERS 		"./utility/[a-zA-Z]*.hpp")
file(GLOB VIDEO_SOURCES 		"./video/[a-zA-Z]*.cpp")
file(GLOB VIDEO_HEADERS 		"./video/[a-zA-Z]*.hpp")

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
	${AUDIO_SOURCES}
	${AUDIO_HEADERS}
	${UTILITY_SOURCES}
	${UTILITY_HEADERS}
	"./types.cpp"
	"./types.hpp"
)

if(WIN32)
	target_sources(leviathan PRIVATE "./resource/icon.rc")
endif()

if(${CMAKE_VERSION} VERSION_GREATER "3.15")
	target_precompile_headers(leviathan PRIVATE "./stdafx.hpp")
endif()
