cmake_minimum_required (VERSION 3.15)

target_sources (leviathan PRIVATE
	"collision.cpp" "collision.hpp"
	"parallax_background.cpp" "parallax_background.hpp"
	"tile_flag.hpp"
	"tilemap_layer.cpp" "tilemap_layer.hpp"
	"tilemap.cpp" "tilemap.hpp"
	"tileset_viewer.cpp" "tileset_viewer.hpp"
)
