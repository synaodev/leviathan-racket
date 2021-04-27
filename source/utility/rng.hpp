#pragma once

#include "../types.hpp"

namespace rng {
	sint64_t seed();
	void seed(sint64_t value);
	sint_t next(sint_t low, sint_t high);
	real_t next(real_t low, real_t high);
}
