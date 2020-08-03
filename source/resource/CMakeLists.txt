cmake_minimum_required (VERSION 3.15)

target_sources (leviathan PRIVATE
	"icon.cpp" "icon.hpp"
	"tbl_entry.hpp" "id.hpp"
	"pipeline.cpp" "pipeline.hpp"
)

if (MSVC)
	target_sources (leviathan PRIVATE "icon.rc")
endif ()
