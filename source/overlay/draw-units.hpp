#pragma once

#include "../utility/enums.hpp"
#include "../utility/rect.hpp"
#include "../video/vertex-pool.hpp"

struct renderer_t;
struct texture_t;

struct draw_units_t : public not_copyable_t {
public:
	draw_units_t();
	draw_units_t(draw_units_t&&) noexcept = default;
	draw_units_t& operator=(draw_units_t&&) noexcept = default;
	~draw_units_t() = default;
public:
	void invalidate() const;
	void render(renderer_t& renderer) const;
	void set_position(real_t x, real_t y);
	void set_position(const glm::vec2& position);
	void set_bounding(real_t x, real_t y, real_t w, real_t h);
	void set_bounding(const rect_t& bounding);
	void set_values(sint_t current, sint_t maximum);
	void set_values(sint_t current, sint_t maximum, sint_t tabular);
	void set_tabular(sint_t tabular);
	void set_texture(const texture_t* texture);
	const glm::vec2& get_position() const;
private:
	void generate(arch_t current, arch_t maximum, bool_t resize);
private:
	mutable bool_t amend { false };
	glm::vec2 position {};
	rect_t bounding {};
	sint_t current_value { 0 };
	sint_t maximum_value { 0 };
	sint_t tabular_value { 0 };
	const texture_t* texture { nullptr };
	vertex_pool_t quads {};
};
