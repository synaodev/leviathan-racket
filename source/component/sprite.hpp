#pragma once

#include <entt/core/hashed_string.hpp>

#include "../utility/rect.hpp"
#include "../utility/enums.hpp"

struct animation_t;
struct renderer_t;
struct kontext_t;

struct sprite_t : public not_copyable_t {
public:
	sprite_t(const entt::hashed_string& entry);
	sprite_t(const animation_t* file) :
		file(file) {}
	sprite_t() = default;
	sprite_t(sprite_t&& that) noexcept : sprite_t() {
		if (this != &that) {
			std::swap(file, that.file);
			std::swap(amend, that.amend);
			std::swap(timer, that.timer);
			std::swap(alpha, that.alpha);
			std::swap(table, that.table);
			std::swap(state, that.state);
			std::swap(variation, that.variation);
			std::swap(mirroring, that.mirroring);
			std::swap(frame, that.frame);
			std::swap(layer, that.layer);
			std::swap(scale, that.scale);
			std::swap(position, that.position);
			std::swap(pivot, that.pivot);
			std::swap(angle, that.angle);
			std::swap(shake, that.shake);
		}
	}
	sprite_t& operator=(sprite_t&& that) noexcept {
		if (this != &that) {
			std::swap(file, that.file);
			std::swap(amend, that.amend);
			std::swap(timer, that.timer);
			std::swap(alpha, that.alpha);
			std::swap(table, that.table);
			std::swap(state, that.state);
			std::swap(variation, that.variation);
			std::swap(mirroring, that.mirroring);
			std::swap(frame, that.frame);
			std::swap(layer, that.layer);
			std::swap(scale, that.scale);
			std::swap(position, that.position);
			std::swap(pivot, that.pivot);
			std::swap(angle, that.angle);
			std::swap(shake, that.shake);
		}
		return *this;
	}
	~sprite_t() = default;
public:
	void reset();
	void new_state(arch_t state);
	glm::vec2 action_point(arch_t state, arch_t variation, mirroring_t mirroring, const glm::vec2& position) const;
	bool finished() const;
public:
	static void update(kontext_t& kontext, real64_t delta);
	static void render(const kontext_t& kontext, renderer_t& renderer, const rect_t& viewport, bool_t panic);
	static bool compare(const sprite_t& lhv, const sprite_t& rhv) {
		return lhv.layer < rhv.layer;
	}
public:
	static constexpr arch_t NonState = (arch_t)-1;
public:
	const animation_t* file { nullptr };
	mutable bool_t amend { false };
	real64_t timer { 0.0 };
	real_t alpha { 1.0f };
	sint_t table { 0 };
	arch_t state { 0 };
	union {
		arch_t variation;
		direction_t direction { direction_t::Right };
		oriented_t oriented;
	};
	mirroring_t mirroring { mirroring_t::None };
	arch_t frame { 0 };
	layer_t layer { layer_value::Automatic };
	glm::vec2 scale { 1.0f };
	glm::vec2 position {};
	glm::vec2 pivot {};
	real_t angle { 0.0f };
	real_t shake { 0.0f };
};
