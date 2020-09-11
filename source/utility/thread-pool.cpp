#include "./thread-pool.hpp"

thread_pool_t::thread_pool_t() :
	shutdown(false),
	queue(),
	threads(),
	conditional_mutex(),
	conditional_lock()
{

}

thread_pool_t::thread_pool_t(arch_t count) : thread_pool_t() {
	this->setup(count);
}

thread_pool_t::~thread_pool_t() {
	this->destroy();
}

void thread_pool_t::setup(arch_t count) {
	threads.resize(count);
	this->reset();
}

void thread_pool_t::reset() {
	for (auto&& thread : threads) {
		thread = std::thread(worker_t(this));
	}
}

void thread_pool_t::destroy() {
	shutdown = true;
	conditional_lock.notify_all();
	for (auto&& thread : threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}
