cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"icon.cpp" "icon.hpp"
	"tbl_entry.hpp" "id.hpp"
	"pipeline.cpp" "pipeline.hpp"
)

target_sources (editor PRIVATE
	"icon.cpp" "icon.hpp"
	"tbl_entry.hpp"
	"pipeline.cpp" "pipeline.hpp"
)

if (WIN32)
	target_sources (naomi PRIVATE "icon.rc")
	target_sources (editor PRIVATE "icon.rc")
endif ()
