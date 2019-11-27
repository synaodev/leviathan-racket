#include "./types.hpp"

#include <random>
#include <chrono>

static
#ifndef SYNAO_MACHINE_x64
std::mt19937_64&
#else // SYNAO_MACHINE_x64
std::mt19937&
#endif // SYNAO_MACHINE_x64
mersenne() {
	static const sint64_t seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	static 
#ifndef SYNAO_MACHINE_x64
		std::mt19937_64 
#else // SYNAO_MACHINE_x64
		std::mt19937
#endif // SYNAO_MACHINE_x64
		generator(
			static_cast<uint_t>(seed)
		);
	return generator;
}

sint_t rng::next(sint_t low, sint_t high) {
	std::uniform_int_distribution<sint_t> dst(low, high);
	return dst(mersenne());
}

real_t rng::next(real_t low, real_t high) {
	std::uniform_real_distribution<real_t> dst(low, high);
	return dst(mersenne());
}