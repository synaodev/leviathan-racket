cmake_minimum_required (VERSION 3.13)

if (NAOMI_BUILD)
	target_sources (naomi PRIVATE
		"common.cpp"
		"friends.cpp"
		"ghost.cpp"
		"naomi.cpp"
		"particles.cpp"
		"shoshi.cpp"
		"weapons.cpp"
	)
endif ()
