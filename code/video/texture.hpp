#ifndef SYNAO_VIDEO_TEXTURE_HPP
#define SYNAO_VIDEO_TEXTURE_HPP

#include <future>
#include <atomic>

#include "./image.hpp"

struct thread_pool_t;

namespace __enum_pixel_format {
	enum type : uint_t {
		Invalid  = 0,
		R3G3B2A0 = 0x2A10,
		R8G8B8A0 = 0x8051,
		R2G2B2A2 = 0x8055,
		R4G4B4A4 = 0x8056,
		R8G8B8A8 = 0x8058
	};
}

using pixel_format_t = __enum_pixel_format::type;

struct sampler_t {
public:
	sampler_t() = default;
	~sampler_t() = default;
public:
	static bool has_immutable_storage();
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
	bool color_buffer(glm::ivec2 dimensions, arch_t layers, pixel_format_t format);
	bool color_buffer_at(glm::ivec2 dimensions, pixel_format_t format, arch_t offset);
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

#endif // SYNAO_VIDEO_TEXTURE_HPP