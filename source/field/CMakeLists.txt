cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"camera.cpp"
	"collision.cpp"
	"properties.cpp"
	"tilemap.cpp"
	"tilemap_layer.cpp"
	"tilemap_parallax.cpp"
)

target_sources (editor PRIVATE "tileset_viewer.cpp")
