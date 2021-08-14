#pragma once

#include <functional>
#include <future>
#include <atomic>
#include <thread>
#include <vector>
#include <queue>

// Based on Jakob Progsch's C++11 thread pool implementation
// https://github.com/progschj/ThreadPool

struct thread_pool_t : public not_copyable_t {
public:
	thread_pool_t() = default;
	thread_pool_t(thread_pool_t&&) noexcept = delete;
	thread_pool_t& operator=(thread_pool_t&&) noexcept = delete;
	~thread_pool_t() {
		shutdown = true;
		if (!threads.empty()) {
			{
				std::unique_lock<std::mutex> notify_lock { conditional_mutex };
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
public:
	bool init(arch_t count) {
		if (threads.empty()) {
			threads.resize(count);
			for (auto&& thread : threads) {
				thread = std::thread(worker_t(this));
			}
			return true;
		}
		return false;
	}
	template<typename Func, typename...Args>
	auto push(Func&& func, Args&& ... args) -> std::future<decltype(func(args...))> {
		std::function<decltype(func(args...))()> process = std::bind(
			std::forward<Func>(func),
			std::forward<Args>(args)...);
		auto task_pointer = std::make_shared<std::packaged_task<decltype(func(args...))()> >(process);
		std::function<void()> wrapper = [task_pointer] { std::invoke(*task_pointer); };
		{
			std::unique_lock<std::mutex> push_lock { queue_mutex };
			queue.push(wrapper);
		}
		conditional_lock.notify_one();
		return task_pointer->get_future();
	}
private:
	struct worker_t : public not_copyable_t {
	public:
		worker_t(thread_pool_t* boss) : boss(boss) {}
		worker_t(worker_t&&) noexcept = default;
		worker_t& operator=(worker_t&&) noexcept = default;
		~worker_t() = default;
	public:
		void operator()() {
			std::function<void()> process {};
			bool dequeued = false;
			while (!boss->shutdown) {
				{
					std::unique_lock<std::mutex> wait_lock { boss->conditional_mutex };
					bool empty = false;
					{
						std::unique_lock<std::mutex> empty_lock { boss->queue_mutex };
						empty = boss->queue.empty();
					}
					if (empty) {
						boss->conditional_lock.wait(wait_lock);
					}
					{
						std::unique_lock<std::mutex> dequeued_lock { boss->queue_mutex };
						if (!boss->queue.empty()) {
							process = std::move(boss->queue.front());
							boss->queue.pop();
							dequeued = true;
						} else {
							dequeued = false;
						}
					}
				}
				if (dequeued) {
					std::invoke(process);
				}
			}
		}
	private:
		thread_pool_t* boss { nullptr };
	};
private:
	std::atomic<bool> shutdown { false };
	std::queue<std::function<void()> > queue {};
	std::mutex queue_mutex {};
	std::vector<std::thread> threads {};
	std::mutex conditional_mutex {};
	std::condition_variable conditional_lock {};
};
