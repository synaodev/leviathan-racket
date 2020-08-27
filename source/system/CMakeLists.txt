cmake_minimum_required (VERSION 3.15)

if (NAOMI)
	target_sources (naomi PRIVATE
		"audio.cpp" "audio.hpp"
		"button.cpp" "button.hpp"
		"input.cpp" "input.hpp"
		"kernel.cpp" "kernel.hpp"
		"main.cpp"
		"music.cpp" "music.hpp"
		"renderer.cpp" "renderer.hpp"
		"runtime.cpp" "runtime.hpp"
		"video.cpp" "video.hpp"
	)
endif ()

if (EDITOR)
	target_sources (editor PRIVATE
		"button.cpp" "button.hpp"
		"input.cpp" "input.hpp"
		"editor.cpp" "editor.hpp"
		"main.cpp"
		"renderer.cpp" "renderer.hpp"
		"video.cpp" "video.hpp"
	)
endif ()
