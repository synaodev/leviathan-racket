#include "./types.hpp"

#include <random>
#include <chrono>

namespace rng {
	static sint64_t& current() {
		static sint64_t s = std::chrono::high_resolution_clock::now()
			.time_since_epoch()
			.count();
		return s;
	}
	static std::mt19937& mersenne() {
		auto& s = rng::current();
		static std::mt19937 generator(static_cast<uint_t>(s));
		return generator;
	}
	sint64_t seed() {
		auto& s = rng::current();
		return s;
	}
	void seed(sint64_t value) {
		auto& s = rng::current();
		s = value;
		auto& m = rng::mersenne();
		m.seed(static_cast<uint_t>(s));
	}
	sint_t next(sint_t low, sint_t high) {
		auto& m = rng::mersenne();
		std::uniform_int_distribution<sint_t> distribution(low, high);
		return distribution(m);
	}
	real_t next(real_t low, real_t high) {
		auto& m = rng::mersenne();
		std::uniform_real_distribution<real_t> distribution(low, high);
		return distribution(m);
	}
}
