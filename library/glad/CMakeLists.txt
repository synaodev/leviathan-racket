cmake_minimum_required (VERSION 3.13)

if (NAOMI_BUILD)
	target_sources (naomi PRIVATE "glad.cpp")
endif ()

if (EDITOR_BUILD)
	target_sources (editor PRIVATE "glad.cpp")
endif ()
