cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"const-buffer.cpp"
	"display-list.cpp"
	"font.cpp"
	"frame-buffer.cpp"
	"gfx.cpp"
	"gl-check.cpp"
	"image.cpp"
	"pipeline.cpp"
	"quad-buffer.cpp"
	"texture.cpp"
	"vertex-pool.cpp"
	"vertex.cpp"
)

target_sources (editor PRIVATE
	"const-buffer.cpp"
	"display-list.cpp"
	"frame-buffer.cpp"
	"gfx.cpp"
	"gl-check.cpp"
	"image.cpp"
	"pipeline.cpp"
	"quad-buffer.cpp"
	"texture.cpp"
	"vertex-pool.cpp"
	"vertex.cpp"
)
