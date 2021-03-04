cmake_minimum_required (VERSION 3.13)

target_sources (lvrk PRIVATE
	"FreeFuncs.cpp"
	"ImageLayer.cpp"
	"LayerGroup.cpp"
	"Map.cpp"
	"miniz.cpp"
	"Object.cpp"
	"ObjectGroup.cpp"
	"Property.cpp"
	"TileLayer.cpp"
	"Tileset.cpp"
)

add_subdirectory ("detail")
