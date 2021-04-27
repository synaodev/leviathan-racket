#pragma once

#include "../types.hpp"

struct kontext_t;

struct blinker_t {
public:
	blinker_t(arch_t first_state, arch_t blink_state) :
		first_state(first_state),
		blink_state(blink_state) {}
	blinker_t() = default;
	blinker_t(const blinker_t&) = default;
	blinker_t& operator=(const blinker_t&) = default;
	blinker_t(blinker_t&&) noexcept = default;
	blinker_t& operator=(blinker_t&&) noexcept = default;
	~blinker_t() = default;
public:
	static void update(kontext_t& kontext, real64_t delta);
public:
	bool_t enable { true };
	real64_t timer { 0.0 };
	arch_t first_state { 0 };
	arch_t blink_state { 0 };
};
