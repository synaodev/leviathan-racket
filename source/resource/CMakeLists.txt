cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE "program.cpp")
target_sources (editor PRIVATE "program.cpp")

if (WIN32)
	target_sources (naomi PRIVATE "icon.rc")
	target_sources (editor PRIVATE "icon.rc")
endif ()
