cmake_minimum_required (VERSION 3.15)

target_sources (leviathan PRIVATE
	"animation_sequence.cpp" "animation_sequence.hpp"
	"animation.cpp" "animation.hpp"
	"const_buffer.cpp" "const_buffer.hpp"
	"depth_buffer.cpp" "depth_buffer.hpp"
	"display_list.cpp" "display_list.hpp"
	"font.cpp" "font.hpp"
	"frame_buffer.cpp" "frame_buffer.hpp"
	"gfx.cpp" "gfx.hpp"
	"glad.cpp" "glad.hpp"
	"glcheck.cpp" "glcheck.hpp"
	"image.cpp" "image.hpp"
	"khrplatform.hpp"
	"light.cpp" "light.hpp"
	"palette.cpp" "palette.hpp"
	"program.cpp" "program.hpp"
	"quad_buffer.cpp" "quad_buffer.hpp"
	"texture.cpp" "texture.hpp"
	"vertex_buffer.cpp" "vertex_buffer.hpp"
	"vertex_pool.cpp" "vertex_pool.hpp"
	"vertex.cpp" "vertex.hpp"
)
