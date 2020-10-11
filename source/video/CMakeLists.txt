cmake_minimum_required (VERSION 3.13)

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
	"sampler.cpp"
	"texture.cpp"
	"vertex-pool.cpp"
	"vertex.cpp"
)
