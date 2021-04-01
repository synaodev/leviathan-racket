#ifndef LEVIATHAN_INCLUDED_UTILITY_WATCH_HPP
#define LEVIATHAN_INCLUDED_UTILITY_WATCH_HPP

#include <chrono>

#include "../types.hpp"

struct watch_t {
public:
	watch_t() : point() {
		point = std::chrono::high_resolution_clock::now();
	}
	watch_t(const watch_t&) = default;
	watch_t(watch_t&&) noexcept = default;
	watch_t& operator=(const watch_t&) = default;
	watch_t& operator=(watch_t&&) noexcept = default;
	~watch_t() = default;
public:
	real64_t elapsed() const {
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::duration<real64_t> >(now - point).count();
	}
	real64_t restart() {
		auto now = std::chrono::high_resolution_clock::now();
		auto delta = now - point;
		point = now;
		return std::chrono::duration_cast<std::chrono::duration<real64_t> >(delta).count();
	}
	static sint64_t timestamp() {
		return std::chrono::system_clock::now().time_since_epoch().count();
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> point {};
};

#endif // LEVIATHAN_INCLUDED_UTILITY_WATCH_HPP
