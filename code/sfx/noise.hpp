#ifndef SYNAO_SOUND_NOISE_HPP
#define SYNAO_SOUND_NOISE_HPP

#include <string>
#include <atomic>
#include <future>
#include <set>

#include "../types.hpp"

struct thread_pool_t;
struct channel_t;

struct noise_t : public not_copyable_t {
public:
	noise_t();
	noise_t(noise_t&& that) noexcept;
	noise_t& operator=(noise_t&& that) noexcept;
	~noise_t();
public:
	bool load(const std::string& full_path);
	bool load(const std::string& full_path, thread_pool_t& thread_pool);
	void destroy();
private:
	friend struct channel_t;
	std::atomic<bool> ready;
	std::future<void> future;
	uint_t handle;
	mutable std::set<channel_t*> binder;
};

#endif // SYNAO_SOUND_NOISE_HPP