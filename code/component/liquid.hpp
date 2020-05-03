#ifndef SYNAO_COMPONENT_LIQUID_HPP
#define SYNAO_COMPONENT_LIQUID_HPP

#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>

#include "../utility/rect.hpp"

struct audio_t;
struct renderer_t;
struct kontext_t;
struct location_t;

struct liquid_body_t {
public:
	liquid_body_t(rect_t hitbox) :
		write(true),
		hitbox(hitbox) {}
	liquid_body_t() : 
		write(true),
		hitbox() {}
	liquid_body_t(const liquid_body_t&) = default;
	liquid_body_t& operator=(const liquid_body_t&) = default;
	liquid_body_t(liquid_body_t&&) = default;
	liquid_body_t& operator=(liquid_body_t&&) = default;
	~liquid_body_t() = default;
public:	
	mutable bool_t write;
	rect_t hitbox;
};

struct liquid_listener_t {
public:
	liquid_listener_t(arch_t particle_type, const byte_t* sound_name) :
		liquid(entt::null),
		particle_type(particle_type),
		sound_name(sound_name) {}
	liquid_listener_t() :
		liquid(entt::null),
		particle_type(0),
		sound_name(nullptr) {}
	liquid_listener_t(const liquid_listener_t&) = default;
	liquid_listener_t& operator=(const liquid_listener_t&) = default;
	liquid_listener_t(liquid_listener_t&&) = default;
	liquid_listener_t& operator=(liquid_listener_t&&) = default;
	~liquid_listener_t() = default;
public:
	entt::entity liquid;
	arch_t particle_type;
	const byte_t* sound_name;
};

namespace liquid {
	void handle(audio_t& audio, kontext_t& kontext, const location_t& location, liquid_listener_t& listener);
	void handle(audio_t& audio, kontext_t& kontext);
	void render(const kontext_t& context, renderer_t& renderer, rect_t viewport);
}

#endif // SYNAO_COMPONENT_LIQUID_HPP 