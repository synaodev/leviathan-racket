cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"animation_sequence.cpp"
	"animation.cpp"
	"const_buffer.cpp"
	"depth_buffer.cpp"
	"display_list.cpp"
	"font.cpp"
	"frame_buffer.cpp"
	"gfx.cpp"
	"glcheck.cpp"
	"image.cpp"
	"light.cpp"
	"palette.cpp"
	"program.cpp"
	"quad_buffer.cpp"
	"texture.cpp"
	"vertex_buffer.cpp"
	"vertex_pool.cpp"
	"vertex.cpp"
)

target_sources (editor PRIVATE
	"const_buffer.cpp"
	"depth_buffer.cpp"
	"display_list.cpp"
	"frame_buffer.cpp"
	"gfx.cpp"
	"glcheck.cpp"
	"image.cpp"
	"palette.cpp"
	"program.cpp"
	"quad_buffer.cpp"
	"texture.cpp"
	"vertex_pool.cpp"
	"vertex.cpp"
)
