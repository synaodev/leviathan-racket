#ifndef LEVIATHAN_INCLUDED_COMPONENT_COMMON_HPP
#define LEVIATHAN_INCLUDED_COMPONENT_COMMON_HPP

#include <array>
#include <bitset>
#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>

#include "../utility/enums.hpp"

namespace __enum_phy {
	enum type : arch_t {
		Right,
		Left,
		Top,
		Bottom,
		Sloped,
		Noclip,
		Outbounds,
		WillDrop,
		FallThrough,
		Hooked,
		Constrained,
		Total
	};
}

using phy_t = __enum_phy::type;

namespace __enum_trigger_flags {
	enum type : arch_t {
		Deterred,
		Hostile,
		InteractionEvent,
		DeathEvent,
		Total
	};
}

using trigger_flags_t = __enum_trigger_flags::type;

struct actor_header_t {
public:
	actor_header_t(arch_t type) :
		type(type),
		attach(entt::null) {}
	actor_header_t() :
		type(0),
		attach(entt::null) {}
	actor_header_t(const actor_header_t&) = default;
	actor_header_t(actor_header_t&&) = default;
	actor_header_t& operator=(const actor_header_t&) = default;
	actor_header_t& operator=(actor_header_t&&) = default;
	~actor_header_t() = default;
public:
	arch_t type;
	entt::entity attach;
};

struct actor_spawn_t {
public:
	actor_spawn_t(arch_t type, glm::vec2 position) :
		type(type),
		position(position),
		velocity(0.0f),
		direction(direction_t::Right),
		identity(0),
		bitmask(0) {}
	actor_spawn_t(arch_t type, glm::vec2 position, glm::vec2 velocity) :
		type(type),
		position(position),
		velocity(velocity),
		direction(direction_t::Right),
		identity(0),
		bitmask(0) {}
	actor_spawn_t(arch_t type, glm::vec2 position, direction_t direction) :
		type(type),
		position(position),
		velocity(0.0f),
		direction(direction),
		identity(0),
		bitmask(0) {}
	actor_spawn_t(arch_t type, glm::vec2 position, glm::vec2 velocity, direction_t direction) :
		type(type),
		position(position),
		velocity(velocity),
		direction(direction),
		identity(0),
		bitmask(0) {}
	actor_spawn_t(arch_t type, glm::vec2 position, direction_t direction, sint_t identity, arch_t flags) :
		type(type),
		position(position),
		velocity(0.0f),
		direction(direction),
		identity(identity),
		bitmask(flags) {}
	actor_spawn_t(arch_t type, glm::vec2 position, glm::vec2 velocity, direction_t direction, sint_t identity, arch_t flags) :
		type(type),
		position(position),
		velocity(velocity),
		direction(direction),
		identity(identity),
		bitmask(flags) {}
	actor_spawn_t() :
		type(0),
		position(0.0f),
		velocity(0.0f),
		direction(direction_t::Right),
		identity(0),
		bitmask(0) {}
	actor_spawn_t(const actor_spawn_t&) = default;
	actor_spawn_t(actor_spawn_t&&) = default;
	actor_spawn_t& operator=(const actor_spawn_t&) = default;
	actor_spawn_t& operator=(actor_spawn_t&&) = default;
	~actor_spawn_t() = default;
public:
	arch_t type;
	glm::vec2 position, velocity;
	direction_t direction;
	sint_t identity;
	std::bitset<trigger_flags_t::Total> bitmask;
};

struct actor_trigger_t {
public:
	actor_trigger_t(sint_t identity, const std::bitset<trigger_flags_t::Total>& bitmask) :
		identity(identity),
		bitmask(bitmask) {}
	actor_trigger_t(sint_t identity, arch_t flags) :
		identity(identity),
		bitmask(flags) {}
	actor_trigger_t() :
		identity(0),
		bitmask(0) {}
	actor_trigger_t(const actor_trigger_t&) = default;
	actor_trigger_t& operator=(const actor_trigger_t&) = default;
	actor_trigger_t(actor_trigger_t&& that) noexcept : actor_trigger_t() {
		if (this != &that) {
			std::swap(bitmask, that.bitmask);
			std::swap(identity, that.identity);
		}
	}
	actor_trigger_t& operator=(actor_trigger_t&& that) noexcept {
		if (this != &that) {
			std::swap(bitmask, that.bitmask);
			std::swap(identity, that.identity);
		}
		return *this;
	}
	~actor_trigger_t() = default;
public:
	sint_t identity;
	std::bitset<trigger_flags_t::Total> bitmask;
};

struct actor_timer_t {
public:
	actor_timer_t() :
		data{ 0, 0, 0, 0 } {}
	actor_timer_t(const actor_timer_t&) = default;
	actor_timer_t& operator=(const actor_timer_t&) = default;
	actor_timer_t(actor_timer_t&&) = default;
	actor_timer_t& operator=(actor_timer_t&&) = default;
	~actor_timer_t() = default;
public:
	sint64_t& operator[](arch_t index) {
		return data[index];
	}
	const sint64_t& operator[](arch_t index) const {
		return data[index];
	}
private:
	std::array<sint64_t, 4> data;
};

#endif // LEVIATHAN_INCLUDED_COMPONENT_COMMON_HPP
