#ifndef LEVIATHAN_INCLUDED_VIDEO_PIPELINE_HPP
#define LEVIATHAN_INCLUDED_VIDEO_PIPELINE_HPP

#include <string>
#include <vector>

#include "./vertex.hpp"
#include "./gfx.hpp"

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
	friend struct pipeline_t;
	uint_t handle;
	shader_stage_t stage;
};

struct pipeline_t : public not_copyable_t {
public:
	pipeline_t();
	pipeline_t(pipeline_t&& that) noexcept;
	pipeline_t& operator=(pipeline_t&& that) noexcept;
	~pipeline_t();
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

#endif // LEVIATHAN_INCLUDED_VIDEO_PIPELINE_HPP
