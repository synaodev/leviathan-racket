cmake_minimum_required (VERSION 3.13)

target_sources (lvrk PRIVATE
	"const-buffer.cpp"
	"display-list.cpp"
	"frame-buffer.cpp"
	"gfx.cpp"
	"gl-check.cpp"
	"pipeline.cpp"
	"quad-buffer.cpp"
	"sampler.cpp"
	"texture.cpp"
	"vertex-pool.cpp"
	"vertex.cpp"
)
