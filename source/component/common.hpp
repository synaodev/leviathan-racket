#ifndef LEVIATHAN_INCLUDED_COMPONENT_COMMON_HPP
#define LEVIATHAN_INCLUDED_COMPONENT_COMMON_HPP

#include <array>
#include <bitset>
#include <glm/vec2.hpp>

#include <entt/core/hashed_string.hpp>
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
	actor_header_t(const entt::hashed_string& type) :
		type(type) {}
	actor_header_t() = default;
	actor_header_t(const actor_header_t&) = default;
	actor_header_t& operator=(const actor_header_t&) = default;
	actor_header_t(actor_header_t&&) noexcept = default;
	actor_header_t& operator=(actor_header_t&&) noexcept = default;
	~actor_header_t() = default;
public:
	entt::hashed_string type {};
	entt::entity attach { entt::null };
};

struct actor_spawn_t {
public:
	actor_spawn_t(const entt::hashed_string& type, const glm::vec2& position) :
		type(type),
		position(position) {}
	actor_spawn_t(const entt::hashed_string& type, const glm::vec2& position, const glm::vec2& velocity) :
		type(type),
		position(position),
		velocity(velocity) {}
	actor_spawn_t(const entt::hashed_string& type, const glm::vec2& position, direction_t direction) :
		type(type),
		position(position),
		direction(direction) {}
	actor_spawn_t(const entt::hashed_string& type, const glm::vec2& position, const glm::vec2& velocity, direction_t direction) :
		type(type),
		position(position),
		velocity(velocity),
		direction(direction) {}
	actor_spawn_t(const entt::hashed_string& type, const glm::vec2& position, direction_t direction, sint_t identity, arch_t flags) :
		type(type),
		position(position),
		direction(direction),
		identity(identity),
		bitmask(flags) {}
	actor_spawn_t(const entt::hashed_string& type, const glm::vec2& position, const glm::vec2& velocity, direction_t direction, sint_t identity, arch_t flags) :
		type(type),
		position(position),
		velocity(velocity),
		direction(direction),
		identity(identity),
		bitmask(flags) {}
	actor_spawn_t() = default;
	actor_spawn_t(const actor_spawn_t&) = default;
	actor_spawn_t& operator=(const actor_spawn_t&) = default;
	actor_spawn_t(actor_spawn_t&&) noexcept = default;
	actor_spawn_t& operator=(actor_spawn_t&&) noexcept = default;
	~actor_spawn_t() = default;
public:
	entt::hashed_string type {};
	glm::vec2 position {};
	glm::vec2 velocity {};
	direction_t direction { direction_t::Right };
	sint_t identity { 0 };
	std::bitset<trigger_flags_t::Total> bitmask { 0 };
};

struct actor_trigger_t {
public:
	actor_trigger_t(sint_t identity, const std::bitset<trigger_flags_t::Total>& bitmask) :
		identity(identity),
		bitmask(bitmask) {}
	actor_trigger_t(sint_t identity, arch_t flags) :
		identity(identity),
		bitmask(flags) {}
	actor_trigger_t() = default;
	actor_trigger_t(const actor_trigger_t&) = default;
	actor_trigger_t& operator=(const actor_trigger_t&) = default;
	actor_trigger_t(actor_trigger_t&&) noexcept = default;
	actor_trigger_t& operator=(actor_trigger_t&&) noexcept = default;
	~actor_trigger_t() = default;
public:
	sint_t identity { 0 };
	std::bitset<trigger_flags_t::Total> bitmask { 0 };
};

struct actor_timer_t {
public:
	actor_timer_t() = default;
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
	std::array<sint64_t, 4> data {
		0, 0, 0, 0
	};
};

#endif // LEVIATHAN_INCLUDED_COMPONENT_COMMON_HPP
