#include "./common.hpp"

#include <entt/entity/entity.hpp>

actor_header_t::actor_header_t(const entt::hashed_string& type) :
	type(type),
	attach(entt::null)
{

}

actor_header_t::actor_header_t() :
	type(),
	attach(entt::null)
{

}

actor_spawn_t::actor_spawn_t(const entt::hashed_string& type, glm::vec2 position) :
	type(type),
	position(position),
	velocity(0.0f),
	direction(direction_t::Right),
	identity(0),
	bitmask(0)
{

}

actor_spawn_t::actor_spawn_t(const entt::hashed_string& type, glm::vec2 position, glm::vec2 velocity) :
	type(type),
	position(position),
	velocity(velocity),
	direction(direction_t::Right),
	identity(0),
	bitmask(0)
{

}
actor_spawn_t::actor_spawn_t(const entt::hashed_string& type, glm::vec2 position, direction_t direction) :
	type(type),
	position(position),
	velocity(0.0f),
	direction(direction),
	identity(0),
	bitmask(0)
{

}

actor_spawn_t::actor_spawn_t(const entt::hashed_string& type, glm::vec2 position, glm::vec2 velocity, direction_t direction) :
	type(type),
	position(position),
	velocity(velocity),
	direction(direction),
	identity(0),
	bitmask(0)
{

}

actor_spawn_t::actor_spawn_t(const entt::hashed_string& type, glm::vec2 position, direction_t direction, sint_t identity, arch_t flags) :
	type(type),
	position(position),
	velocity(0.0f),
	direction(direction),
	identity(identity),
	bitmask(flags)
{

}

actor_spawn_t::actor_spawn_t(const entt::hashed_string& type, glm::vec2 position, glm::vec2 velocity, direction_t direction, sint_t identity, arch_t flags) :
	type(type),
	position(position),
	velocity(velocity),
	direction(direction),
	identity(identity),
	bitmask(flags)
{

}

actor_spawn_t::actor_spawn_t() :
	type(),
	position(0.0f),
	velocity(0.0f),
	direction(direction_t::Right),
	identity(0),
	bitmask(0)
{

}

actor_trigger_t::actor_trigger_t(sint_t identity, const std::bitset<trigger_flags_t::Total>& bitmask) :
	identity(identity),
	bitmask(bitmask)
{

}

actor_trigger_t::actor_trigger_t(sint_t identity, arch_t flags) :
	identity(identity),
	bitmask(flags)
{

}

actor_trigger_t::actor_trigger_t() :
	identity(0),
	bitmask(0)
{

}

actor_trigger_t::actor_trigger_t(actor_trigger_t&& that) noexcept : actor_trigger_t() {
	if (this != &that) {
		std::swap(identity, that.identity);
		std::swap(bitmask, that.bitmask);
	}
}

actor_trigger_t& actor_trigger_t::operator=(actor_trigger_t&& that) noexcept {
	if (this != &that) {
		std::swap(identity, that.identity);
		std::swap(bitmask, that.bitmask);
	}
	return *this;
}

actor_timer_t::actor_timer_t() : data{} {
	data.fill(0);
}

sint64_t& actor_timer_t::operator[](arch_t index) {
	return data[index];
}

const sint64_t& actor_timer_t::operator[](arch_t index) const {
	return data[index];
}
