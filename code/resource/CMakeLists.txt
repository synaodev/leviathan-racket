cmake_minimum_required (VERSION 3.15)

target_sources (leviathan PRIVATE
	"icon.cpp" "icon.hpp"
	"id.cpp" "id.hpp"
	"pipeline.cpp" "pipeline.hpp"
)

if (MSVC)
	target_sources (leviathan PRIVATE "icon.rc")
endif ()
