cmake_minimum_required (VERSION 3.15)

if (NAOMI)
	target_sources (naomi PRIVATE
		"blinker.cpp"
		"health.cpp"
		"kinematics.cpp"
		"kontext.cpp"
		"liquid.cpp"
		"location.cpp"
		"routine.cpp"
		"sprite.cpp"
	)
endif ()
