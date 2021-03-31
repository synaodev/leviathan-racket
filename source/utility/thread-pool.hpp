#ifndef LEVIATHAN_INCLUDED_UTILITY_THREAD_POOL_HPP
#define LEVIATHAN_INCLUDED_UTILITY_THREAD_POOL_HPP

#include <functional>
#include <future>
#include <atomic>
#include <thread>
#include <vector>

#include "./safe-queue.hpp"

// Based on Jakob Progsch's C++11 thread pool implementation
// https://github.com/progschj/ThreadPool

struct thread_pool_t : public not_copyable_t {
public:
	thread_pool_t() :
		shutdown(false),
		queue(),
		threads(),
		conditional_mutex(),
		conditional_lock() {}
	thread_pool_t(const thread_pool_t&) = delete;
	thread_pool_t(thread_pool_t&&) = delete;
	thread_pool_t& operator=(const thread_pool_t&) = delete;
	thread_pool_t& operator=(thread_pool_t&&) = delete;
	~thread_pool_t() {
		if (!threads.empty()) {
			shutdown = true;
			conditional_lock.notify_all();
			for (auto&& thread : threads) {
				if (thread.joinable()) {
					thread.join();
				}
			}
		}
	}
public:
	bool init(arch_t count) {
		threads.resize(count);
		for (auto&& thread : threads) {
			thread = std::thread(worker_t(this));
		}
		return !threads.empty();
	}
	void destroy() {
		shutdown = true;
		if (!threads.empty()) {
			{
				std::unique_lock<std::mutex> lock{ conditional_mutex };
				conditional_lock.notify_all();
			}
			for (auto&& thread : threads) {
				if (thread.joinable()) {
					thread.join();
				}
			}
			threads.clear();
		}
	}
	template<typename Func, typename...Args>
	auto push(Func&& func, Args&& ... args) -> std::future<decltype(func(args...))> {
		std::function<decltype(func(args...))()> process = std::bind(
			std::forward<Func>(func),
			std::forward<Args>(args)...);
		auto task_pointer = std::make_shared<std::packaged_task<decltype(func(args...))()> >(process);
		std::function<void()> wrapper = [task_pointer] {
			(*task_pointer)();
		};
		queue.enqueue(wrapper);
		conditional_lock.notify_one();
		return task_pointer->get_future();
	}
private:
	struct worker_t {
	public:
		worker_t(thread_pool_t* thread_pool) : thread_pool(thread_pool) {}
		worker_t() : worker_t(nullptr) {}
		worker_t(const worker_t&) = default;
		worker_t(worker_t&&) = default;
		worker_t& operator=(const worker_t&) = default;
		worker_t& operator=(worker_t&&) = default;
		~worker_t() = default;
	public:
		void operator()() {
			std::function<void()> process;
			bool dequeued;
			while (!thread_pool->shutdown) {
				{
					std::unique_lock<std::mutex> lock{ thread_pool->conditional_mutex };
					if (thread_pool->queue.empty()) {
						thread_pool->conditional_lock.wait(lock);
					}
					dequeued = thread_pool->queue.dequeue(process);
				}
				if (dequeued) {
					process();
				}
			}
		}
	private:
		thread_pool_t* thread_pool;
	};
private:
	std::atomic<bool> shutdown;
	safe_queue_t<std::function<void()> > queue;
	std::vector<std::thread> threads;
	std::mutex conditional_mutex;
	std::condition_variable conditional_lock;
};

#endif // LEVIATHAN_INCLUDED_UTILITY_THREAD_POOL_HPP
