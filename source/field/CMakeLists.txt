cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"camera.cpp"
	"collision.cpp"
	"properties.cpp"
	"tilemap.cpp"
	"tilemap-layer.cpp"
	"tilemap-parallax.cpp"
)

target_sources (editor PRIVATE "tileset-viewer.cpp")
