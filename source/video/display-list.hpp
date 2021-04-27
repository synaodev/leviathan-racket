#pragma once

#include "../utility/enums.hpp"

#include "./gfx.hpp"
#include "./vertex-pool.hpp"
#include "./quad-buffer.hpp"

struct texture_t;
struct palette_t;
struct pipeline_t;
struct rect_t;

struct display_list_t : public not_copyable_t {
public:
	display_list_t(layer_t layer, blend_mode_t blend_mode, const pipeline_t* pipeline) :
		layer(layer),
		blend_mode(blend_mode),
		pipeline(pipeline) {}
	display_list_t() = default;
	// The default move constructors don't play nice with std::sort
	display_list_t(display_list_t&& that) noexcept : display_list_t() {
		if (this != &that) {
			std::swap(layer, that.layer);
			std::swap(blend_mode, that.blend_mode);
			std::swap(pipeline, that.pipeline);
			std::swap(visible, that.visible);
			std::swap(amend, that.amend);
			std::swap(current, that.current);
			std::swap(account, that.account);
			std::swap(quad_pool, that.quad_pool);
			std::swap(quad_buffer, that.quad_buffer);
		}
	}
	display_list_t& operator=(display_list_t&& that) noexcept {
		if (this != &that) {
			std::swap(layer, that.layer);
			std::swap(blend_mode, that.blend_mode);
			std::swap(pipeline, that.pipeline);
			std::swap(visible, that.visible);
			std::swap(amend, that.amend);
			std::swap(current, that.current);
			std::swap(account, that.account);
			std::swap(quad_pool, that.quad_pool);
			std::swap(quad_buffer, that.quad_buffer);
		}
		return *this;
	}
	~display_list_t() = default;
public:
	display_list_t& setup(const quad_allocator_t* allocator);
	display_list_t& begin(arch_t count);
	display_list_t& vtx_pool_write(const vertex_pool_t& that_pool);
	display_list_t& vtx_blank_write(const rect_t& raster_rect, const glm::vec4& vtx_color);
	display_list_t& vtx_major_write(const rect_t& texture_rect, const glm::vec2& raster_dimensions, mirroring_t mirroring, real_t alpha_color, sint_t texture_name, sint_t palette_name);
	display_list_t& vtx_fonts_write(const rect_t& texture_rect, const glm::vec2& raster_dimensions, const glm::vec4& full_color, sint_t atlas_name, sint_t atlas_table);
	display_list_t& vtx_transform_write(const glm::vec2& position, const glm::vec2& scale, const glm::vec2& axis, real_t rotation);
	display_list_t& vtx_transform_write(const glm::vec2& position, const glm::vec2& axis, real_t rotation);
	display_list_t& vtx_transform_write(const glm::vec2& position, const glm::vec2& scale);
	display_list_t& vtx_transform_write(const glm::vec2& position);
	display_list_t& vtx_transform_write(real_t x, real_t y);
	void end();
	void skip(arch_t count);
	void skip();
	void flush(gfx_t& gfx, const sampler_allocator_t* samplers);
	bool matches(layer_t layer, blend_mode_t blend_mode, const pipeline_t* pipeline) const;
	bool rendered() const;
	friend bool operator<(const display_list_t& lhv, const display_list_t& rhv);
public:
	static constexpr arch_t SingleQuad = 4;
private:
	layer_t layer { layer_value::Automatic };
	blend_mode_t blend_mode { blend_mode_t::None };
	const pipeline_t* pipeline { nullptr };
	bool_t visible { false };
	bool_t amend { false };
	arch_t current { 0 };
	arch_t account { 0 };
	vertex_pool_t quad_pool {};
	quad_buffer_t quad_buffer {};
};

bool operator<(const display_list_t& lhv, const display_list_t& rhv);
