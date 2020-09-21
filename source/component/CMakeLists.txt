cmake_minimum_required (VERSION 3.13)

if (NAOMI_BUILD)
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
