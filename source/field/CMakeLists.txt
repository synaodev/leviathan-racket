cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"camera.cpp"
	"collision.cpp"
	"parallax_background.cpp"
	"properties.cpp"
	"tilemap_layer.cpp"
	"tilemap.cpp"
)

target_sources (editor PRIVATE "tileset_viewer.cpp")
