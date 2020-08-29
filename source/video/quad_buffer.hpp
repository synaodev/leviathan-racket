#ifndef LEVIATHAN_INCLUDED_VIDEO_QUAD_BUFFER_HPP
#define LEVIATHAN_INCLUDED_VIDEO_QUAD_BUFFER_HPP

#include <vector>

#include "./gfx.hpp"
#include "./vertex.hpp"

struct quad_buffer_t;

struct quad_buffer_allocator_t : public not_copyable_t {
public:
	quad_buffer_allocator_t();
	quad_buffer_allocator_t(quad_buffer_allocator_t&& that) noexcept;
	quad_buffer_allocator_t& operator=(quad_buffer_allocator_t&& that) noexcept;
	~quad_buffer_allocator_t();
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
	primitive_t primitive;
	uint_t handle;
	arch_t length;
};

struct quad_buffer_t : public not_copyable_t {
public:
	quad_buffer_t();
	quad_buffer_t(quad_buffer_t&& that) noexcept;
	quad_buffer_t& operator=(quad_buffer_t&& that) noexcept;
	~quad_buffer_t();
public:
	// template<typename V>
	// inline void setup(const quad_buffer_allocator_t* allocator, buffer_usage_t usage) {
	// 	static_assert(std::is_base_of<vertex_t, V>::value);
	// 	vertex_spec_t specify = vertex_spec_t::from(typeid(V));
	// 	this->setup(allocator, usage, specify);
	// }
	void setup(const quad_buffer_allocator_t* allocator, buffer_usage_t usage, vertex_spec_t specify);
	void create(arch_t length);
	void destroy();
	bool update(const vertex_t* vertices, arch_t count, arch_t offset);
	bool update(const vertex_t* vertices, arch_t count);
	bool update(const vertex_t* vertices);
	void draw(arch_t count) const;
	void draw() const;
	buffer_usage_t get_usage() const;
	arch_t get_length() const;
private:
	friend struct gfx_t;
	const quad_buffer_allocator_t* allocator;
	buffer_usage_t usage;
	vertex_spec_t specify;
	uint_t arrays, buffer;
	arch_t length;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_QUAD_BUFFER_HPP
