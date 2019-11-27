#ifndef SYNAO_COMPONENT_SPRITE_HPP
#define SYNAO_COMPONENT_SPRITE_HPP

#include <string>

#include "../utl/rect.hpp"
#include "../utl/enums.hpp"

#include "../res_id.hpp"

struct animation_t;
struct renderer_t;
struct kontext_t;

struct sprite_t {
public:
	sprite_t(const std::string& name);
	sprite_t();
	sprite_t(const sprite_t&) = default;
	sprite_t& operator=(const sprite_t&) = default;
	sprite_t(sprite_t&&) = default;
	sprite_t& operator=(sprite_t&&) = default;
	~sprite_t() = default;
public:
	void reset();
	void new_state(arch_t state);
	glm::vec2 action_point(arch_t state, direction_t direction, glm::vec2 position) const;
	bool finished() const;
public:
	static void update(kontext_t& kontext, real64_t delta);
	static void render(const kontext_t& kontext, renderer_t& renderer, rect_t viewport, bool_t panic);
	static bool compare(const sprite_t& lhv, const sprite_t& rhv) {
		return lhv.layer < rhv.layer;
	}
private:
	const animation_t* file;
public:
	static constexpr arch_t NonState = (arch_t)-1;
	mutable bool_t write;
	real64_t timer;
	real_t alpha, table;
	arch_t state;
	direction_t direction;
	arch_t frame;
	layer_t layer;
	glm::vec2 scale, position, pivot;
	real_t angle, shake, power;
};

#endif // SYNAO_COMPONENT_SPRITE_HPP