#ifndef SYNAO_UTILITY_THREAD_POOL_HPP
#define SYNAO_UTILITY_THREAD_POOL_HPP

#include <functional>
#include <future>
#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <vector>

#include "../types.hpp"

struct thread_pool_t : public not_copyable_t, public not_moveable_t {
public:
	thread_pool_t(arch_t count);
	thread_pool_t();
	thread_pool_t(const thread_pool_t&) = delete;
	thread_pool_t(thread_pool_t&&) = delete;
	thread_pool_t& operator=(const thread_pool_t&) = delete;
	thread_pool_t& operator=(thread_pool_t&&) = delete;
	~thread_pool_t();
public:
	void setup(arch_t count);
	void reset();
	void destroy();
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
template<typename T>
	struct safe_queue_t {
	public:
		safe_queue_t() = default;
		safe_queue_t(const safe_queue_t&) = default;
		safe_queue_t(safe_queue_t&&) = default;
		safe_queue_t& operator=(const safe_queue_t&) = default;
		safe_queue_t& operator=(safe_queue_t&&) = default;
		~safe_queue_t() = default;
		bool empty() {
			std::unique_lock<std::mutex> lock{ mutex };
			return queue.empty();
		}
		size_t size() {
			std::unique_lock<std::mutex> lock{ mutex };
			return queue.size();
		}
		void enqueue(T& t) {
			std::unique_lock<std::mutex> lock{ mutex };
			queue.push(t);
		}
		bool dequeue(T& t) {
			std::unique_lock<std::mutex> lock{ mutex };
			if (queue.empty()) {
				return false;
			}
			t = std::move(queue.front());
			queue.pop();
			return true;
		}
	private:
		std::queue<T> queue;
		std::mutex mutex;
	};
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

#endif // SYNAO_UTILITY_THREAD_POOL_HPP