cmake_minimum_required (VERSION 3.15)

if (NAOMI_BUILD)
	target_sources (naomi PRIVATE
		"rng.cpp"
		"setup-file.cpp"
		"thread-pool.cpp"
		"utf32.cpp"
		"vfs.cpp"
	)
endif ()

if (EDITOR_BUILD)
	target_sources (editor PRIVATE
		"rng.cpp"
		"setup-file.cpp"
		"thread-pool.cpp"
		"utf32.cpp"
		"vfs.cpp"
	)
endif ()
