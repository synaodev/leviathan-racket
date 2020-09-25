#ifndef LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP
#define LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP

#include <future>
#include <atomic>
#include <map>

#include "./image.hpp"
#include "./gfx.hpp"

struct thread_pool_t;

struct sampler_t {
public:
	sampler_t() = default;
	~sampler_t() = default;
public:
	static bool has_immutable_option();
	static bool has_azdo();
};

struct sampler_data_t : public not_copyable_t {
public:
	sampler_data_t() :
		id(0),
		type(0),
		count(0) {}
	sampler_data_t(sampler_data_t&&);
	sampler_data_t& operator=(sampler_data_t&&);
	~sampler_data_t();
public:
	uint_t id, type;
	sint_t count;
};

struct sampler_allocator_t : public not_copyable_t {
public:
	sampler_allocator_t() : handles() {}
	sampler_allocator_t(sampler_allocator_t&& that) noexcept;
	sampler_allocator_t& operator=(sampler_allocator_t&& that) noexcept;
	~sampler_allocator_t() = default;
public:
	sampler_data_t& get(const glm::ivec2& dimensions, pixel_format_t format);
	const sampler_data_t& get(const glm::ivec2& dimensions) const;
private:
	std::map<glm::ivec2, sampler_data_t> handles;
};

struct texture_t : public not_copyable_t, public sampler_t {
public:
	texture_t() :
		ready(false),
		future(),
		allocator(nullptr),
		format(pixel_format_t::Invalid),
		name(0),
		dimensions(0) {}
	texture_t(texture_t&& that) noexcept;
	texture_t& operator=(texture_t&& that) noexcept;
	~texture_t();
public:
	void load(const std::string& full_path, pixel_format_t format, sampler_allocator_t& allocator, thread_pool_t& thread_pool);
	void assure();
	void assure() const;
	void destroy();
	bool valid() const;
	uint_t get_handle() const;
	sint_t get_name() const;
	real_t get_convert(real_t index) const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_inverse_dimensions() const;
	glm::ivec2 get_integral_dimensions() const;
private:
	friend struct gfx_t;
	std::atomic<bool> ready;
	std::future<image_t> future;
	sampler_allocator_t* allocator;
	pixel_format_t format;
	sint_t name;
	glm::ivec2 dimensions;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP
