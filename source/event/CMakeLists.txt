cmake_minimum_required (VERSION 3.15)

if (NAOMI)
	target_sources (naomi PRIVATE
		"array.cpp"
		"receiver.cpp"
		"string.cpp"
	)
endif ()
