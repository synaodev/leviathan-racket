#ifndef SYNAO_VIDEO_GFX_HPP
#define SYNAO_VIDEO_GFX_HPP

#include <array>

#include "../types.hpp"

namespace __enum_blend_mode {
	enum type : arch_t {
		Disable,
		None,
		Alpha,
		Add,
		Subtract,
		Multiply
	};
}

using blend_mode_t = __enum_blend_mode::type;

namespace __enum_compare_func {
	enum type : arch_t {
		Disable,
		Never,
		Less,
		Equal,
		Lequal,
		Greater,
		Nequal,
		Gequal,
		Always
	};
}

using compare_func_t = __enum_compare_func::type;

namespace __enum_buffer_usage {
	enum type : arch_t {
		Static,
		Dynamic,
		Stream
	};
}

using buffer_usage_t = __enum_buffer_usage::type;

namespace __enum_primitive {
	enum type : uint_t {
		Points,
		Lines,
		LineLoop,
		LineStrip,
		Triangles,
		TriangleStrip,
		TriangleFan
	};
}

using primitive_t = __enum_primitive::type;

namespace __enum_pixel_format {
	enum type : arch_t {
		Invalid,
		R3G3B2A0,
		R8G8B8A0,
		R2G2B2A2,
		R4G4B4A4,
		R8G8B8A8
	};
}

using pixel_format_t = __enum_pixel_format::type;

struct sampler_t;
struct texture_t;
struct palette_t;
struct depth_buffer_t;
struct program_t;
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
	void set_program(const program_t* program);
	void set_sampler(const texture_t* texture, arch_t index);
	void set_sampler(const palette_t* palette, arch_t index);
	void set_sampler(const depth_buffer_t* depth_buffer, arch_t index);
	void set_sampler(std::nullptr_t, arch_t index);
	void set_const_buffer(const const_buffer_t* buffer, arch_t index);
public:
	static uint_t get_compare_func_gl_enum(compare_func_t func);
	static uint_t get_buffer_usage_gl_enum(buffer_usage_t usage);
	static uint_t get_pixel_format_gl_enum(pixel_format_t format);
private:
	compare_func_t depth_func;
	blend_mode_t blend_mode;
	const program_t* program;
	std::array<const sampler_t*, 4> samplers;
	std::array<const const_buffer_t*, 4> const_buffers;
};

#endif // SYNAO_VIDEO_GFX_HPP
