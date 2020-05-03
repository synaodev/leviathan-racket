#ifndef SYNAO_VIDEO_QUAD_BATCH_HPP
#define SYNAO_VIDEO_QUAD_BATCH_HPP

#include "../utility/enums.hpp"

#include "./gfx.hpp"
#include "./vertex_pool.hpp"
#include "./quad_list.hpp"

struct texture_t;
struct palette_t;
struct program_t;
struct rect_t;

struct quad_batch_t : public not_copyable_t {
public:
	quad_batch_t(layer_t layer, blend_mode_t blend_mode, const texture_t* texture, const palette_t* palette, const program_t* program);
	quad_batch_t();
	quad_batch_t(quad_batch_t&& that) noexcept;
	quad_batch_t& operator=(quad_batch_t&& that) noexcept;
	~quad_batch_t() = default;
public:
	quad_batch_t& begin(arch_t count);
	quad_batch_t& vtx_pool_write(const vertex_pool_t* that_pool);
	quad_batch_t& vtx_major_write(rect_t texture_rect, glm::vec2 raster_dimensions, real_t table_index, real_t alpha_color, mirroring_t mirroring);
	quad_batch_t& vtx_blank_write(rect_t raster_rect, glm::vec4 vtx_color);
	quad_batch_t& vtx_transform_write(glm::vec2 position, glm::vec2 scale, glm::vec2 axis, real_t rotation);
	quad_batch_t& vtx_transform_write(glm::vec2 position, glm::vec2 axis, real_t rotation);
	quad_batch_t& vtx_transform_write(glm::vec2 position, glm::vec2 scale);
	quad_batch_t& vtx_transform_write(glm::vec2 position);
	quad_batch_t& vtx_transform_write(real_t x, real_t y);
	void end();
	void skip(arch_t count);
	void skip();
	void flush(gfx_t* gfx);
	bool matches(layer_t layer, blend_mode_t blend_mode, const texture_t* texture, const palette_t* palette, const program_t* program) const;
	bool visible() const;
	friend bool operator<(const quad_batch_t& lhv, const quad_batch_t& rhv);
public:
	static constexpr arch_t SingleQuad = 4;
private:
	layer_t layer;
	blend_mode_t blend_mode;
	const texture_t* texture;
	const palette_t* palette;
	const program_t* program;
	bool_t drawn, write;
	arch_t current, account;
	vertex_pool_t quad_pool;
	quad_list_t quad_list;
};

bool operator<(const quad_batch_t& lhv, const quad_batch_t& rhv);

#endif // SYNAO_VIDEO_QUAD_BATCH_HPP