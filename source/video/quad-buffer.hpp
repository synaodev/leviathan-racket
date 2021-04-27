#ifndef LEVIATHAN_INCLUDED_VIDEO_QUAD_BUFFER_HPP
#define LEVIATHAN_INCLUDED_VIDEO_QUAD_BUFFER_HPP

#include <vector>

#include "./gfx.hpp"
#include "./vertex.hpp"

struct quad_buffer_t;

struct quad_allocator_t : public not_copyable_t {
public:
	quad_allocator_t() = default;
	quad_allocator_t(quad_allocator_t&&) noexcept = default;
	quad_allocator_t& operator=(quad_allocator_t&&) noexcept = default;
	~quad_allocator_t() {
		this->destroy();
	}
public:
	bool create(primitive_t primitive, arch_t length);
	void destroy();
	void bind(bool_t value) const;
	bool valid() const;
	arch_t get_length() const;
	primitive_t get_primitive() const;
public:
	static arch_t convert(arch_t length);
	static std::vector<uint16_t> generate(arch_t length, arch_t offset, primitive_t primitive);
private:
	primitive_t primitive { primitive_t::Triangles };
	uint_t handle { 0 };
	arch_t length { 0 };
};

struct quad_buffer_t : public not_copyable_t {
public:
	quad_buffer_t() = default;
	quad_buffer_t(quad_buffer_t&&) noexcept = default;
	quad_buffer_t& operator=(quad_buffer_t&&) noexcept = default;
	~quad_buffer_t() {
		this->destroy();
	}
public:
	void setup(const quad_allocator_t* allocator, buffer_usage_t usage, vertex_spec_t specify);
	void create(arch_t length);
	void destroy();
	bool update(const vertex_t* vertices, arch_t count, arch_t offset);
	bool update(const vertex_t* vertices, arch_t count);
	bool update(const vertex_t* vertices);
	void draw(arch_t count) const;
	void draw() const;
	buffer_usage_t get_usage() const;
	arch_t get_length() const;
	bool valid() const;
private:
	friend struct gfx_t;
	const quad_allocator_t* allocator { nullptr };
	buffer_usage_t usage { buffer_usage_t::Static };
	vertex_spec_t specify {};
	uint_t arrays { 0 };
	uint_t buffer { 0 };
	arch_t length { 0 };
};

#endif // LEVIATHAN_INCLUDED_VIDEO_QUAD_BUFFER_HPP
