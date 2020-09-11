#ifndef LEVIATHAN_INCLUDED_VIDEO_DISPLAY_LIST_HPP
#define LEVIATHAN_INCLUDED_VIDEO_DISPLAY_LIST_HPP

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
	display_list_t(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const texture_t* texture, const palette_t* palette, const pipeline_t* pipeline, const quad_allocator_t* allocator);
	display_list_t();
	display_list_t(display_list_t&& that) noexcept;
	display_list_t& operator=(display_list_t&& that) noexcept;
	~display_list_t() = default;
public:
	display_list_t& begin(arch_t count);
	display_list_t& vtx_pool_write(const vertex_pool_t& that_pool);
	display_list_t& vtx_major_write(rect_t texture_rect, glm::vec2 raster_dimensions, real_t table_index, real_t alpha_color, mirroring_t mirroring);
	display_list_t& vtx_blank_write(rect_t raster_rect, glm::vec4 vtx_color);
	display_list_t& vtx_transform_write(glm::vec2 position, glm::vec2 scale, glm::vec2 axis, real_t rotation);
	display_list_t& vtx_transform_write(glm::vec2 position, glm::vec2 axis, real_t rotation);
	display_list_t& vtx_transform_write(glm::vec2 position, glm::vec2 scale);
	display_list_t& vtx_transform_write(glm::vec2 position);
	display_list_t& vtx_transform_write(real_t x, real_t y);
	void end();
	void skip(arch_t count);
	void skip();
	void flush(gfx_t& gfx);
	sint64_t capture(const gfx_t& gfx);
	bool release(const gfx_t& gfx);
	bool matches(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const texture_t* texture, const palette_t* palette, const pipeline_t* pipeline) const;
	bool matches(sint64_t timestamp) const;
	bool rendered() const;
	bool persists() const;
	friend bool operator<(const display_list_t& lhv, const display_list_t& rhv);
public:
	static constexpr arch_t SingleQuad = 4;
private:
	layer_t layer;
	blend_mode_t blend_mode;
	const texture_t* texture;
	const palette_t* palette;
	const pipeline_t* pipeline;
	bool_t visible, amend;
	sint64_t timestamp;
	arch_t current, account;
	vertex_pool_t quad_pool;
	quad_buffer_t quad_buffer;
};

bool operator<(const display_list_t& lhv, const display_list_t& rhv);

#endif // LEVIATHAN_INCLUDED_VIDEO_DISPLAY_LIST_HPP
