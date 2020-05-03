#ifndef SYNAO_OVERLAY_DRAW_UNITS_HPP
#define SYNAO_OVERLAY_DRAW_UNITS_HPP

#include "../utility/enums.hpp"
#include "../utility/rect.hpp"
#include "../graphics/vertex_pool.hpp"

struct renderer_t;
struct texture_t;
struct palette_t;

struct draw_units_t : public not_copyable_t {
public:
	draw_units_t();
	draw_units_t(draw_units_t&&) = default;
	draw_units_t& operator=(draw_units_t&&) = default;
	~draw_units_t() = default;
public:
	void force() const;
	void render(renderer_t& renderer) const;
	void set_position(real_t x, real_t y);
	void set_position(glm::vec2 position);
	void set_bounding(real_t x, real_t y, real_t w, real_t h);
	void set_bounding(rect_t bounding);
	void set_values(sint_t current, sint_t maximum);
	void set_table(real_t table);
	void set_texture(const texture_t* texture);
	void set_palette(const palette_t* palette);
	glm::vec2 get_position() const;
private:
	void generate(arch_t current, arch_t maximum, bool_t resize);
private:
	mutable bool_t write;
	glm::vec2 position;
	rect_t bounding;
	sint_t current_value, maximum_value;
	real_t table;
	const texture_t* texture;
	const palette_t* palette;
	vertex_pool_t quads;
};

#endif // SYNAO_OVERLAY_DRAW_UNITS_HPP