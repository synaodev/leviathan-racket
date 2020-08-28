cmake_minimum_required (VERSION 3.15)

if (NAOMI)
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

if (EDITOR)
	target_sources (editor PRIVATE
		"input.cpp"
		"editor.cpp"
		"main.cpp"
		"renderer.cpp"
		"video.cpp"
	)
endif ()
