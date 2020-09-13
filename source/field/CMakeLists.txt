cmake_minimum_required (VERSION 3.15)

if (NAOMI_BUILD)
	target_sources (naomi PRIVATE
		"camera.cpp"
		"collision.cpp"
		"properties.cpp"
		"tilemap.cpp"
		"tilemap-layer.cpp"
		"tilemap-parallax.cpp"
	)
endif ()

if (EDITOR_BUILD)
	target_sources (editor PRIVATE "tileset-viewer.cpp")
endif ()
