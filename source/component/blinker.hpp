#ifndef SYNAO_COMPONENT_BLINKER_HPP
#define SYNAO_COMPONENT_BLINKER_HPP

#include "../types.hpp"

struct kontext_t;

struct blinker_t {
public:
	blinker_t(arch_t first_state, arch_t blink_state);
	blinker_t();
	blinker_t(const blinker_t&) = default;
	blinker_t& operator=(const blinker_t&) = default;
	blinker_t(blinker_t&&) = default;
	blinker_t& operator=(blinker_t&&) = default;
	~blinker_t() = default;
public:
	static void update(kontext_t& kontext, real64_t delta);
public:
	bool_t enable;
	real64_t timer;
	arch_t first_state;
	arch_t blink_state;
};

#endif // SYNAO_COMPONENT_BLINKER_HPP