#ifndef SYNAO_UTILITY_CLOCK_HPP
#define SYNAO_UTILITY_CLOCK_HPP

#include <chrono>

#include "../types.hpp"

struct watch_t {
public:
	watch_t();
	watch_t(const watch_t&) = default;
	watch_t(watch_t&&) = default;
	watch_t& operator=(const watch_t&) = default;
	watch_t& operator=(watch_t&&) = default;
	~watch_t() = default;
public:
	real64_t elapsed() const;
	real64_t restart();
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> point;
};

#endif // SYNAO_UTILITY_CLOCK_HPP