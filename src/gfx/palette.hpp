#ifndef SYNAO_GRAPHICS_PALETTE_HPP
#define SYNAO_GRAPHICS_PALETTE_HPP

#include "./texture.hpp"

struct palette_t : public not_copyable_t, public sampler_t {
public:
	palette_t();
	palette_t(palette_t&& that) noexcept;
	palette_t& operator=(palette_t&& that) noexcept;
	~palette_t();
public:
	bool load(const std::string& full_path, pixel_format_t format, thread_pool_t& thread_pool);
	void assure();
	void assure() const;
	void destroy();
private:
	friend struct gfx_t;
	std::atomic<bool> ready;
	std::future<image_t> future;
	uint_t handle;
	glm::ivec2 dimensions;
	pixel_format_t format;
};

#endif // SYNAO_GRAPHICS_PALETTE_HPP