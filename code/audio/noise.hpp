#ifndef SYNAO_AUDIO_NOISE_HPP
#define SYNAO_AUDIO_NOISE_HPP

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
	void load(const std::string& full_path);
	void load(const std::string& full_path, thread_pool_t& thread_pool);
	bool create();
	void destroy();
	// void assure();
	void assure() const;
private:
	friend struct channel_t;
	std::atomic<bool> ready;
	// SDL_AudioSpec spec;
	std::future<void> future;
	// std::future<std::vector<uint8_t> > future;
	uint_t handle;
	mutable std::set<channel_t*> binder;
};

#endif // SYNAO_AUDIO_NOISE_HPP