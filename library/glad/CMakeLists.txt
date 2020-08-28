cmake_minimum_required (VERSION 3.15)

if (NAOMI)
	target_sources (naomi PRIVATE "glad.cpp")
endif ()

if (EDITOR)
	target_sources (editor PRIVATE "glad.cpp")
endif ()
