#ifndef LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP
#define LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP

#include <future>
#include <atomic>

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

struct texture_t : public not_copyable_t, public sampler_t {
public:
	texture_t();
	texture_t(texture_t&& that) noexcept;
	texture_t& operator=(texture_t&& that) noexcept;
	~texture_t();
public:
	void load(const std::vector<std::string>& full_paths, pixel_format_t format, thread_pool_t& thread_pool);
	bool create(glm::ivec2 dimensions, arch_t layers, pixel_format_t format);
	void assure();
	void assure() const;
	void destroy();
	bool valid() const;
	uint_t get_layers() const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_inverse_dimensions() const;
	glm::ivec2 get_integral_dimensions() const;
private:
	friend struct gfx_t;
	std::atomic<bool> ready;
	std::future<std::vector<image_t> > future;
	uint_t handle;
	glm::ivec2 dimensions;
	arch_t layers;
	pixel_format_t format;
};

struct palette_t : public not_copyable_t, public sampler_t {
public:
	palette_t();
	palette_t(palette_t&& that) noexcept;
	palette_t& operator=(palette_t&& that) noexcept;
	~palette_t();
public:
	void load(const std::string& full_path, pixel_format_t format, thread_pool_t& thread_pool);
	bool create(glm::ivec2 dimensions, pixel_format_t format);
	void destroy();
	void assure();
	void assure() const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_inverse_dimensions() const;
	glm::ivec2 get_integral_dimensions() const;
	real_t convert(real_t index) const;
private:
	friend struct gfx_t;
	std::atomic<bool> ready;
	std::future<image_t> future;
	uint_t handle;
	glm::ivec2 dimensions;
	pixel_format_t format;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_TEXTURE_HPP
