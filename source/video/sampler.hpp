#pragma once

#include "./gfx.hpp"

struct sampler_t {
public:
	static sint_t get_working_unit();
	static sint_t get_maximum_textures();
	static sint_t get_maximum_atlases();
	static bool has_immutable_option();
};

struct sampler_data_t : public not_copyable_t {
public:
	sampler_data_t(uint_t id, uint_t type, uint_t count) :
		id(id),
		type(type),
		count(count) {}
	sampler_data_t() = default;
	sampler_data_t(sampler_data_t&&) noexcept = default;
	sampler_data_t& operator=(sampler_data_t&&) noexcept = default;
	~sampler_data_t() {
		this->destroy();
	}
public:
	void destroy();
public:
	uint_t id { 0 };
	uint_t type { 0 };
	sint_t count { 0 };
};

struct sampler_allocator_t : public not_copyable_t {
public:
	sampler_allocator_t() = default;
	sampler_allocator_t(sampler_allocator_t&& that) noexcept = default;
	sampler_allocator_t& operator=(sampler_allocator_t&& that) noexcept = default;
	~sampler_allocator_t() = default;
public:
	bool create(pixel_format_t highest, pixel_format_t lowest);
	sampler_data_t& texture(const glm::ivec2& dimensions);
	const sampler_data_t& texture() const;
	sampler_data_t& atlas(const glm::ivec2& dimensions);
	const sampler_data_t& atlas() const;
private:
	static sampler_data_t kNullHandle;
private:
	friend struct gfx_t;
	pixel_format_t highest { pixel_format_t::R8G8B8A8 };
	pixel_format_t lowest { pixel_format_t::R8G8B8A8 };
	sampler_data_t textures {};
	sampler_data_t atlases {};
};
