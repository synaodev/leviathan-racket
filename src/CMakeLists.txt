cmake_minimum_required(VERSION 3.16.1)

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
file(GLOB IMGUI_SOURCES 		"./gui/[a-zA-Z]*.cpp")
file(GLOB IMGUI_HEADERS 		"./gui/[a-zA-Z]*.h")
file(GLOB MENU_SOURCES 			"./mnu/[a-zA-Z]*.cpp")
file(GLOB MENU_HEADERS 			"./mnu/[a-zA-Z]*.hpp")
file(GLOB OVERLAY_SOURCES		"./oly/[a-zA-Z]*.cpp")
file(GLOB OVERLAY_HEADERS		"./oly/[a-zA-Z]*.hpp")
file(GLOB PXTONE_SOURCES		"./pxt/[a-zA-Z]*.cpp")
file(GLOB PXTONE_HEADERS		"./pxt/[a-zA-Z]*.h")
file(GLOB SOUND_SOURCES 		"./sfx/[a-zA-Z]*.cpp")
file(GLOB SOUND_HEADERS 		"./sfx/[a-zA-Z]*.hpp")
file(GLOB SYSTEM_SOURCES 		"./sys/[a-zA-Z]*.cpp")
file(GLOB SYSTEM_HEADERS 		"./sys/[a-zA-Z]*.hpp")
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
	"${IMGUI_SOURCES}"
	"${IMGUI_HEADERS}"
	"${MENU_SOURCES}"
	"${MENU_HEADERS}"
	"${OVERLAY_SOURCES}"
	"${OVERLAY_HEADERS}"
	"${PXTONE_SOURCES}"
	"${PXTONE_HEADERS}"
	"${SOUND_SOURCES}"
	"${SOUND_HEADERS}"
	"${SYSTEM_SOURCES}"
	"${SYSTEM_HEADERS}"
	"${UTILITY_SOURCES}"
	"${UTILITY_HEADERS}"
	"${BASIC_SOURCES}"
	"${BASIC_HEADERS}"
)

if(WIN32)
	target_sources(leviathan PUBLIC "./icon.rc")
	target_precompile_headers(leviathan PUBLIC "./stdafx.hxx")
endif()