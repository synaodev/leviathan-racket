#ifndef SYNAO_VIDEO_PROGRAM_HPP
#define SYNAO_VIDEO_PROGRAM_HPP

#include <string>
#include <vector>

#include "./vertex.hpp"

namespace __enum_shader_stage {
	enum type : uint_t {
		Vertex	 = 0x8B31,
		Geometry = 0x8DD9,
		Fragment = 0x8B30
	};
}

using shader_stage_t = __enum_shader_stage::type;

struct shader_t : public not_copyable_t {
public:
	shader_t();
	shader_t(shader_t&& that) noexcept;
	shader_t& operator=(shader_t&& that) noexcept;
	~shader_t();
public:
	bool load(const std::string& full_path, shader_stage_t stage);
	bool from(const std::string& source, shader_stage_t stage);
	void destroy();
	bool matches(shader_stage_t _stage) const;
	static vertex_spec_t attributes(uint_t program_handle);
private:
	friend struct program_t;
	uint_t handle;
	shader_stage_t stage;
};

struct program_t : public not_copyable_t {
public:
	program_t();
	program_t(program_t&& that) noexcept;
	program_t& operator=(program_t&& that) noexcept;
	~program_t();
public:
	bool create(const shader_t* vert, const shader_t* frag, const shader_t* geom);
	bool create(const shader_t* vert, const shader_t* frag);
	void destroy();
	void set_block(const byte_t* name, arch_t binding) const;
	void set_sampler(const byte_t* name, arch_t sampler) const;
	const vertex_spec_t& get_specify() const;
	static bool has_separable();
	static bool has_uniform_azdo();
private:
	friend struct gfx_t;
	uint_t handle;
	vertex_spec_t specify;
};

#endif // SYNAO_VIDEO_PROGRAM_HPP