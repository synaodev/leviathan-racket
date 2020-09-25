#ifndef LEVIATHAN_INCLUDED_VIDEO_GFX_HPP
#define LEVIATHAN_INCLUDED_VIDEO_GFX_HPP

#include <array>

#include "../types.hpp"

enum class blend_mode_t {
	Disable, None, Alpha,
	Add, Subtract, Multiply
};

enum class compare_func_t {
	Disable, Never, Less,
	Equal, Lequal, Greater,
	Nequal, Gequal, Always
};

enum class buffer_usage_t {
	Static, Dynamic, Stream
};

enum class primitive_t {
	Points, Lines, LineLoop,
	LineStrip, Triangles,
	TriangleStrip, TriangleFan
};

enum class pixel_format_t {
	Invalid,
	R3G3B2A0,
	R8G8B8A0,
	R2G2B2A2,
	R4G4B4A4,
	R8G8B8A8
};

enum class shader_stage_t {
	Vertex,
	Geometry,
	Fragment
};

struct pipeline_t;
struct sampler_t;
struct texture_t;
struct palette_t;
struct color_buffer_t;
struct depth_buffer_t;
struct const_buffer_t;
struct frame_buffer_t;

struct gfx_t : public not_copyable_t {
public:
	gfx_t();
	gfx_t(gfx_t&&) noexcept = default;
	gfx_t& operator=(gfx_t&&) noexcept = default;
	~gfx_t() = default;
public:
	void set_depth_func(compare_func_t depth_func);
	void set_blend_mode(blend_mode_t blend_mode);
	void set_pipeline(const pipeline_t* pipeline);
	void set_sampler(const texture_t* texture, arch_t index);
	void set_sampler(const palette_t* palette, arch_t index);
	void set_sampler(const color_buffer_t* texture, arch_t index);
	void set_sampler(const depth_buffer_t* depth_buffer, arch_t index);
	void set_sampler(std::nullptr_t, arch_t index);
	void set_const_buffer(const const_buffer_t* buffer, arch_t index);
public:
	static uint_t get_compare_func_gl_enum(compare_func_t func);
	static uint_t get_buffer_usage_gl_enum(buffer_usage_t usage);
	static uint_t get_primitive_gl_enum(primitive_t primitive);
	static uint_t get_pixel_format_gl_enum(pixel_format_t format);
	static uint_t get_shader_stage_gl_enum(shader_stage_t stage);
private:
	compare_func_t depth_func;
	blend_mode_t blend_mode;
	const pipeline_t* pipeline;
	std::array<const sampler_t*, 4> samplers;
	std::array<const const_buffer_t*, 4> const_buffers;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_GFX_HPP
