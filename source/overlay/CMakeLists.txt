cmake_minimum_required (VERSION 3.13)

if (NAOMI_BUILD)
	target_sources (naomi PRIVATE
		"draw-count.cpp"
		"draw-fade.cpp"
		"draw-item-view.cpp"
		"draw-meter.cpp"
		"draw-scheme.cpp"
		"draw-text.cpp"
		"draw-title-view.cpp"
		"draw-units.cpp"
	)
endif ()
