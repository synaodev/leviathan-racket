cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"camera.hpp" "camera.cpp"
	"collision.cpp" "collision.hpp"
	"parallax_background.cpp" "parallax_background.hpp"
	"tileflag.hpp"
	"tilemap_layer.cpp" "tilemap_layer.hpp"
	"tilemap.cpp" "tilemap.hpp"
)

target_sources (editor PRIVATE
	"tileset_viewer.cpp" "tileset_viewer.hpp" 
)
