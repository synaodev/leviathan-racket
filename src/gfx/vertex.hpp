#ifndef SYNAO_GRAPHICS_VERTEX_HPP
#define SYNAO_GRAPHICS_VERTEX_HPP

#include <typeinfo>

#include "../types.hpp"

struct vertex_spec_t {
public:
	void(*detail)(void);
	arch_t length;
public:
	vertex_spec_t();
	vertex_spec_t(const vertex_spec_t& that);
	vertex_spec_t(vertex_spec_t&& that) noexcept;
	vertex_spec_t& operator=(const vertex_spec_t& that);
	vertex_spec_t& operator=(vertex_spec_t&& that) noexcept;
	~vertex_spec_t() = default;
public:
	static bool compare(const uint_t* lhv, const uint_t* rhv);
	static vertex_spec_t from(const uint_t* list);
	static vertex_spec_t from(const std::type_info& info);
	bool operator==(const vertex_spec_t& that);
	bool operator!=(const vertex_spec_t& that);
};

struct vtx_minor_t {
public:
	glm::vec2 position;
public:
	vtx_minor_t(real_t x, real_t y) : 
		position(x, y) {}
	vtx_minor_t() :
		position(0.0f) {}
	~vtx_minor_t() = default;
};

struct vtx_blank_t : public vtx_minor_t {
	glm::vec4 color;
public:
	vtx_blank_t() : 
		vtx_minor_t(),
		color(1.0f) {}
	~vtx_blank_t() = default;
};

struct vtx_major_t : public vtx_minor_t {
	glm::vec2 uvcoords;
	real_t table, alpha;
public:
	vtx_major_t() : 
		vtx_minor_t(), 
		uvcoords(0.0f), 
		table(0.0f), 
		alpha(0.0f) {}
	~vtx_major_t() = default;
};

#endif // SYNAO_GRAPHICS_VERTEX_HPP