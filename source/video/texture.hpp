#ifndef LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP
#define LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP

#include <future>
#include <atomic>
#include <array>

#include "./sampler.hpp"
#include "../resource/image.hpp"

struct thread_pool_t;

struct texture_t : public not_copyable_t, public not_moveable_t, public sampler_t {
public:
	texture_t() = default;
	~texture_t() {
		if (future.valid()) {
			auto result = future.get();
		}
	}
public:
	void load(const std::string& full_path, sampler_allocator_t* allocator, thread_pool_t& thread_pool);
	void assure();
	void assure() const;
	bool valid() const;
	uint_t get_handle() const;
	sint_t get_name() const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_inverse_dimensions() const;
	glm::ivec2 get_integral_dimensions() const;
private:
	friend struct gfx_t;
	std::atomic<bool> ready { false };
	std::future<image_t> future {};
	sampler_allocator_t* allocator { nullptr };
	sint_t name { 0 };
	glm::ivec2 dimensions {};
};

struct atlas_t : public not_copyable_t, public not_moveable_t, public sampler_t {
public:
	atlas_t() = default;
	~atlas_t() {
		if (future.valid()) {
			auto result = future.get();
		}
	}
public:
	void load(const std::string& full_path, sampler_allocator_t* allocator, thread_pool_t& thread_pool);
	void assure();
	void assure() const;
	bool valid() const;
	uint_t get_handle() const;
	sint_t get_name() const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_inverse_dimensions() const;
	glm::ivec2 get_integral_dimensions() const;
private:
	friend struct gfx_t;
	std::atomic<bool> ready { false };
	std::future<image_t> future {};
	sampler_allocator_t* allocator { nullptr };
	sint_t name { 0 };
	glm::ivec2 dimensions {};
};

struct palette_t : public not_copyable_t, public sampler_t {
public:
	palette_t() = default;
	~palette_t() {
		if (future.valid()) {
			auto result = future.get();
		}
	}
public:
	void load(const std::string& full_path, sampler_allocator_t* allocator, thread_pool_t& thread_pool);
	void assure();
	void assure() const;
	bool valid() const;
	real_t convert(real_t index) const;
	uint_t get_handle() const;
	sint_t get_name() const;
private:
	friend struct gfx_t;
	std::atomic<bool> ready { false };
	std::future<image_t> future {};
	sampler_allocator_t* allocator { nullptr };
	sint_t name { 0 };
	glm::ivec2 dimensions {};
};

#endif // LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP
