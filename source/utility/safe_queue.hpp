#ifndef LEVIATHAN_INCLUDED_UTILITY_SAFE_QUEUE_HPP
#define LEVIATHAN_INCLUDED_UTILITY_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>

#include "../types.hpp"

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

#endif // LEVIATHAN_INCLUDED_UTILITY_SAFE_QUEUE_HPP
