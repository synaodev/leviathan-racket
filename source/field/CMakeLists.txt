cmake_minimum_required (VERSION 3.15)

if (NAOMI)
	target_sources (naomi PRIVATE
		"camera.cpp"
		"collision.cpp"
		"parallax_background.cpp"
		"tilemap_layer.cpp"
		"tilemap.cpp"
	)
endif ()

if (EDITOR)
	target_sources (editor PRIVATE "tileset_viewer.cpp")
endif ()
