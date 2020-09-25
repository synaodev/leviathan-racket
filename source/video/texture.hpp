#ifndef LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP
#define LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP

#include <future>
#include <atomic>
#include <array>

#include "./image.hpp"
#include "./gfx.hpp"

struct thread_pool_t;

struct sampler_t {
public:
	static bool has_immutable_option();
};

struct sampler_data_t : public not_copyable_t {
public:
	sampler_data_t() :
		id(0),
		count(0) {}
	sampler_data_t(uint_t id, uint_t count) :
		id(id),
		count(count) {}
	sampler_data_t(sampler_data_t&&) noexcept;
	sampler_data_t& operator=(sampler_data_t&&) noexcept;
	~sampler_data_t();
public:
	uint_t id;
	sint_t count;
};

struct sampler_allocator_t : public not_copyable_t {
public:
	sampler_allocator_t() :
		highest(pixel_format_t::Invalid),
		lowest(pixel_format_t::Invalid),
		texsmall(),
		texlarge(),
		palettes() {}
	sampler_allocator_t(pixel_format_t highest, pixel_format_t lowest) :
		highest(highest),
		lowest(lowest),
		texsmall(),
		texlarge(),
		palettes() {}
	sampler_allocator_t(sampler_allocator_t&& that) noexcept;
	sampler_allocator_t& operator=(sampler_allocator_t&& that) noexcept;
	~sampler_allocator_t() = default;
public:
	sampler_data_t& texture(const glm::ivec2& dimensions);
	const sampler_data_t& texture_const(const glm::ivec2& dimensions) const;
	sampler_data_t& palette(const glm::ivec2& dimensions);
	const sampler_data_t& palette_const(const glm::ivec2& dimensions) const;
private:
	static sampler_data_t generate(const glm::ivec2& dimensions, sint_t layers, pixel_format_t format);
private:
	pixel_format_t highest, lowest;
	sampler_data_t texsmall, texlarge, palettes;
};

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
	void load(const std::string& full_path, sampler_allocator_t& allocator, thread_pool_t& thread_pool);
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
	void load(const std::string& full_path, sampler_allocator_t& allocator, thread_pool_t& thread_pool);
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
