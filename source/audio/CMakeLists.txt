cmake_minimum_required (VERSION 3.15)

if (NAOMI)
	target_sources (naomi PRIVATE
		"alcheck.cpp"
		"channel.cpp"
		"noise.cpp"
	)
endif ()
