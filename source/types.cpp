#include "./types.hpp"

#include <random>
#include <chrono>

#ifdef LEVIATHAN_MACHINE_64BIT
	using mersenne_t = std::mt19937_64;
#else
	using mersenne_t = std::mt19937;
#endif

static mersenne_t& get_mersenne() {
	static const sint64_t seed = std::chrono::high_resolution_clock::now()
		.time_since_epoch()
		.count();
	static mersenne_t generator(static_cast<uint_t>(seed));
	return generator;
}

sint_t rng::next(sint_t low, sint_t high) {
	std::uniform_int_distribution<sint_t> distribution(low, high);
	return distribution(get_mersenne());
}

real_t rng::next(real_t low, real_t high) {
	std::uniform_real_distribution<real_t> distribution(low, high);
	return distribution(get_mersenne());
}
