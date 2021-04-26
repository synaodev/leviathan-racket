#ifndef LEVIATHAN_INCLUDED_COMPONENT_LIQUID_HPP
#define LEVIATHAN_INCLUDED_COMPONENT_LIQUID_HPP

#include <entt/core/hashed_string.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>

#include "../utility/rect.hpp"

struct audio_t;
struct renderer_t;
struct kontext_t;
struct location_t;

struct liquid_body_t {
public:
	liquid_body_t(const rect_t& hitbox) :
		hitbox(hitbox) {}
	liquid_body_t() = default;
	liquid_body_t(const liquid_body_t&) = default;
	liquid_body_t& operator=(const liquid_body_t&) = default;
	liquid_body_t(liquid_body_t&&) noexcept = default;
	liquid_body_t& operator=(liquid_body_t&&) noexcept = default;
	~liquid_body_t() = default;
public:
	mutable bool_t amend { true };
	rect_t hitbox {};
};

struct liquid_listener_t {
public:
	liquid_listener_t(const entt::hashed_string& particle, const entt::hashed_string& sound) :
		particle(particle),
		sound(sound) {}
	liquid_listener_t() = default;
	liquid_listener_t(const liquid_listener_t&) = default;
	liquid_listener_t& operator=(const liquid_listener_t&) = default;
	liquid_listener_t(liquid_listener_t&&) noexcept = default;
	liquid_listener_t& operator=(liquid_listener_t&&) noexcept = default;
	~liquid_listener_t() = default;
public:
	entt::entity liquid { entt::null };
	entt::hashed_string particle {};
	entt::hashed_string sound {};
};

namespace liquid {
	void handle(audio_t& audio, kontext_t& kontext, const location_t& location, liquid_listener_t& listener);
	void handle(audio_t& audio, kontext_t& kontext);
	void render(const kontext_t& context, renderer_t& renderer, rect_t viewport);
}

#endif // LEVIATHAN_INCLUDED_COMPONENT_LIQUID_HPP
