cmake_minimum_required (VERSION 3.15)

if (NAOMI)
	target_sources (naomi PRIVATE
		"array.cpp" "array.hpp"
		"receiver.cpp" "receiver.hpp"
		"string.cpp" "string.hpp"
	)
endif ()
