#ifndef LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP
#define LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP

#include <future>
#include <atomic>
#include <array>

#include "./image.hpp"
#include "./sampler.hpp"

struct thread_pool_t;

struct texture_t : public not_copyable_t, public sampler_t {
public:
	texture_t() :
		ready(false),
		future(),
		allocator(nullptr),
		name(0),
		dimensions(0) {}
	texture_t(texture_t&& that) noexcept;
	texture_t& operator=(texture_t&& that) noexcept;
	~texture_t();
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
	std::atomic<bool> ready;
	std::future<image_t> future;
	sampler_allocator_t* allocator;
	sint_t name;
	glm::ivec2 dimensions;
};

struct atlas_t : public not_copyable_t, public sampler_t {
public:
	atlas_t() :
		ready(false),
		future(),
		allocator(nullptr),
		name(0),
		dimensions(0) {}
	atlas_t(atlas_t&& that) noexcept;
	atlas_t& operator=(atlas_t&& that) noexcept;
	~atlas_t();
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
	std::atomic<bool> ready;
	std::future<image_t> future;
	sampler_allocator_t* allocator;
	sint_t name;
	glm::ivec2 dimensions;
};

struct palette_t : public not_copyable_t, public sampler_t {
public:
	palette_t() :
		ready(false),
		future(),
		allocator(nullptr),
		name(0),
		dimensions(0) {}
	palette_t(palette_t&& that) noexcept;
	palette_t& operator=(palette_t&& that) noexcept;
	~palette_t();
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
	std::atomic<bool> ready;
	std::future<image_t> future;
	sampler_allocator_t* allocator;
	sint_t name;
	glm::ivec2 dimensions;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP
