cmake_minimum_required (VERSION 3.13)

if (NAOMI_BUILD)
	target_sources (naomi PRIVATE
		"audio.cpp"
		"input.cpp"
		"kernel.cpp"
		"main.cpp"
		"music.cpp"
		"receiver.cpp"
		"renderer.cpp"
		"runtime.cpp"
		"video.cpp"
	)
endif ()

if (EDITOR_BUILD)
	target_sources (editor PRIVATE
		"input.cpp"
		"editor.cpp"
		"main.cpp"
		"renderer.cpp"
		"video.cpp"
	)
endif ()
