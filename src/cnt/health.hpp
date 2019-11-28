#ifndef SYNAO_COMPONENT_HEALTH_HPP
#define SYNAO_COMPONENT_HEALTH_HPP

#include <bitset>

#include "../types.hpp"

namespace __enum_health_flags {
	enum type : arch_t {
		Hurt,
		Attack,
		Leviathan,
		Invincible,
		Deflectable,
		Hookable,
		Grappled,
		Instant,
		OnceMore,
		Total
	};
}

using health_flags_t = __enum_health_flags::type;

struct audio_t;
struct receiver_t;
struct naomi_state_t;
struct kontext_t;

struct health_t {
public:
	health_t() : 
		flags(0),
		current(2),
		maximum(2),
		leviathan(0),
		damage(0) {}
	health_t(const health_t&) = default;
	health_t& operator=(const health_t&) = default;
	health_t(health_t&&) = default;
	health_t& operator=(health_t&&) = default;
	~health_t() = default;
public:
	void reset(sint_t current, sint_t maximum, sint_t leviathan, sint_t damage);
	void attack(health_t& victim) const;
public:
	static void handle(audio_t& audio, receiver_t& receiver, naomi_state_t& naomi_state, kontext_t& kontext);
public:
	std::bitset<health_flags_t::Total> flags;
	sint_t current, maximum;
	sint_t leviathan, damage;
};

#endif // SYNAO_COMPONENT_HEALTH_HPP