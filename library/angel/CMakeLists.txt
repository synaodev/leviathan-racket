cmake_minimum_required (VERSION 3.13)

if (NAOMI_BUILD)
	target_sources (naomi PRIVATE
		"scriptarray.cpp"
		"scriptstdstring.cpp"
	)
endif ()
