#ifndef LEVIATHAN_INCLUDED_COMPONENT_SPRITE_HPP
#define LEVIATHAN_INCLUDED_COMPONENT_SPRITE_HPP

#include <entt/core/hashed_string.hpp>

#include "../utility/rect.hpp"
#include "../utility/enums.hpp"

struct animation_t;
struct renderer_t;
struct kontext_t;

struct sprite_t {
public:
	sprite_t(const entt::hashed_string& entry);
	sprite_t(const animation_t* file);
	sprite_t();
	sprite_t(const sprite_t&) = default;
	sprite_t& operator=(const sprite_t&) = default;
	sprite_t(sprite_t&&) = default;
	sprite_t& operator=(sprite_t&&) = default;
	~sprite_t() = default;
public:
	void reset();
	void new_state(arch_t state);
	glm::vec2 action_point(arch_t state, arch_t variation, mirroring_t mirroring, glm::vec2 position) const;
	bool finished() const;
public:
	static void update(kontext_t& kontext, real64_t delta);
	static void render(const kontext_t& kontext, renderer_t& renderer, rect_t viewport, bool_t panic);
	static bool compare(const sprite_t& lhv, const sprite_t& rhv) {
		return lhv.layer < rhv.layer;
	}
public:
	static constexpr arch_t NonState = (arch_t)-1;
	const animation_t* file;
	mutable bool_t amend;
	real64_t timer;
	real_t alpha;
	sint_t table;
	arch_t state;
	union {
		arch_t variation;
		direction_t direction;
		oriented_t oriented;
	};
	mirroring_t mirroring;
	arch_t frame;
	layer_t layer;
	glm::vec2 scale, position, pivot;
	real_t angle, shake;
};

#endif // LEVIATHAN_INCLUDED_COMPONENT_SPRITE_HPP
