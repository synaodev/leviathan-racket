#include "./watch.hpp"

watch_t::watch_t() :
	point()
{
	point = std::chrono::high_resolution_clock::now();
}

real64_t watch_t::elapsed() const {
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::duration<real64_t> >(now - point).count();
}

real64_t watch_t::restart() {
	auto now = std::chrono::high_resolution_clock::now();
	auto delta = now - point;
	point = now;
	return std::chrono::duration_cast<std::chrono::duration<real64_t> >(delta).count();
}

sint64_t watch_t::timestamp() {
	return std::chrono::system_clock::now().time_since_epoch().count();
}
