cmake_minimum_required (VERSION 3.13)

if (NAOMI_BUILD)
	target_sources (naomi PRIVATE
		"al-check.cpp"
		"channel.cpp"
		"noise.cpp"
	)
endif ()
