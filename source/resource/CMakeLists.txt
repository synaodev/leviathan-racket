cmake_minimum_required (VERSION 3.13)

target_sources (naomi PRIVATE "animation.cpp" "program.cpp")

if (WIN32)
	target_sources (naomi PRIVATE "icon.rc")
endif ()
