#pragma once

#include <string>
#include <atomic>
#include <future>
#include <set>

#include "../types.hpp"

struct thread_pool_t;
struct channel_t;

struct noise_t : public not_copyable_t, public not_moveable_t {
public:
	noise_t() = default;
	~noise_t() {
		this->destroy();
	}
public:
	void load(const std::string& full_path);
	void load(const std::string& full_path, thread_pool_t& thread_pool);
	bool create();
	void destroy();
	void assure() const;
	bool error() const;
private:
	friend struct channel_t;
	std::atomic<bool> ready { false };
	std::future<void> future {};
	uint_t handle { 0 };
	mutable std::set<channel_t*> binder {};
};
