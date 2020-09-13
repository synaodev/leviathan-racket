cmake_minimum_required (VERSION 3.15)

if (NAOMI_BUILD)
	target_sources (naomi PRIVATE
		"dialogue-gui.cpp"
		"headsup-gui.cpp"
		"inventory-gui.cpp"
		"meta-state.cpp"
		"stack-gui.cpp"
		"wgt-audio.cpp"
		"wgt-file.cpp"
		"wgt-input.cpp"
		"wgt-language.cpp"
		"wgt-option.cpp"
		"wgt-video.cpp"
		"widget.cpp"
	)
endif ()

if (EDITOR_BUILD)
	target_sources (editor PRIVATE "attribute-gui.cpp")
endif ()
