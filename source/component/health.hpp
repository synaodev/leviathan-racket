#pragma once

#include <bitset>

#include "../types.hpp"

struct audio_t;
struct receiver_t;
struct naomi_state_t;
struct kontext_t;

struct health_t {
public:
	health_t() = default;
	health_t(const health_t&) = default;
	health_t& operator=(const health_t&) = default;
	health_t(health_t&&) noexcept = default;
	health_t& operator=(health_t&&) noexcept = default;
	~health_t() = default;
public:
	void reset(sint_t current, sint_t maximum, sint_t leviathan, sint_t damage);
	void reset();
	void attack(health_t& victim) const;
public:
	static void handle(audio_t& audio, receiver_t& receiver, naomi_state_t& naomi, kontext_t& kontext);
public:
	enum flags_t : arch_t {
		Hurt,
		Attack,
		Leviathan,
		Invincible,
		Deflectable,
		Hookable,
		Grappled,
		Instant,
		OnceMore,
		MajorFight,
		TotalFlags
	};
public:
	std::bitset<flags_t::TotalFlags> flags { 0 };
	sint_t current { 2 };
	sint_t maximum { 2 };
	sint_t leviathan { 0 };
	sint_t damage { 0 };
};
