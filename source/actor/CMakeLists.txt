cmake_minimum_required (VERSION 3.15)

if (NAOMI)
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
