#ifndef SYNAO_VIDEO_GFX_HPP
#define SYNAO_VIDEO_GFX_HPP

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

namespace __enum_gfx_cmp_func {
	enum type : uint_t {
		Disable	= 0x0000,
		Never	= 0x0200,
		Less	= 0x0201,
		Equal	= 0x0202,
		Lequal	= 0x0203,
		Greater	= 0x0204,
		Nequal	= 0x0205,
		Gequal	= 0x0206,
		Always	= 0x0207
	};
}

using gfx_cmp_func_t = __enum_gfx_cmp_func::type;

struct sampler_t;
struct texture_t;
struct palette_t;
struct depth_buffer_t;
struct program_t;
struct const_buffer_t;

struct gfx_t : public not_copyable_t {
public:
	gfx_t();
	gfx_t(gfx_t&&) = default;
	gfx_t& operator=(gfx_t&&) = default; 
	~gfx_t() = default;
public:
	void set_depth_func(gfx_cmp_func_t depth_func);
	void set_blend_mode(blend_mode_t blend_mode);
	void set_program(const program_t* program);
	void set_sampler(const texture_t* texture, arch_t index);
	void set_sampler(const palette_t* palette, arch_t index);
	void set_sampler(const depth_buffer_t* depth_buffer, arch_t index);
	void set_sampler(std::nullptr_t, arch_t index);
	void set_buffer(const const_buffer_t* buffer, arch_t index);
private:
	static constexpr arch_t kSmpUnits = 4;
	static constexpr arch_t kBufUnits = 4;
	gfx_cmp_func_t depth_func;
	blend_mode_t blend_mode;
	const program_t* program;
	const sampler_t* samplers[kSmpUnits];
	const const_buffer_t* buffers[kBufUnits];
};

#endif // SYNAO_VIDEO_GFX_HPP