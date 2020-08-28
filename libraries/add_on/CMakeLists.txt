cmake_minimum_required (VERSION 3.15)

if (NAOMI)
	target_sources (naomi PRIVATE
		"scriptarray.cpp"
		"scriptstdstring.cpp"
	)
endif ()
