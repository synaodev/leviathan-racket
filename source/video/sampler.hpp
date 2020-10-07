#ifndef LEVIATHAN_INCLUDED_VIDEO_SAMPLER_HPP
#define LEVIATHAN_INCLUDED_VIDEO_SAMPLER_HPP

#include "./gfx.hpp"

struct sampler_t {
public:
	static bool has_immutable_option();
	static sint_t get_working_unit();
	static sint_t get_maximum_textures();
	static sint_t get_maximum_palettes();
};

struct sampler_data_t : public not_copyable_t {
public:
	sampler_data_t() :
		id(0),
		type(0),
		count(0) {}
	sampler_data_t(uint_t id, uint_t type, uint_t count) :
		id(id),
		type(type),
		count(count) {}
	sampler_data_t(sampler_data_t&&) noexcept;
	sampler_data_t& operator=(sampler_data_t&&) noexcept;
	~sampler_data_t();
public:
	uint_t id, type;
	sint_t count;
};

struct sampler_allocator_t : public not_copyable_t {
public:
	sampler_allocator_t() :
		highest(pixel_format_t::Invalid),
		lowest(pixel_format_t::Invalid),
		textures(),
		palettes() {}
	sampler_allocator_t(pixel_format_t highest, pixel_format_t lowest) :
		highest(highest),
		lowest(lowest),
		textures(),
		palettes() {}
	sampler_allocator_t(sampler_allocator_t&& that) noexcept;
	sampler_allocator_t& operator=(sampler_allocator_t&& that) noexcept;
	~sampler_allocator_t() = default;
public:
	sampler_data_t& texture(const glm::ivec2& dimensions);
	const sampler_data_t& texture() const;
	sampler_data_t& palette(const glm::ivec2& dimensions);
	const sampler_data_t& palette() const;
private:
	friend struct gfx_t;
	pixel_format_t highest, lowest;
	sampler_data_t textures, palettes;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_SAMPLER_HPP
