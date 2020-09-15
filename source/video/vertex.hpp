#ifndef LEVIATHAN_INCLUDED_VIDEO_VERTEX_HPP
#define LEVIATHAN_INCLUDED_VIDEO_VERTEX_HPP

#include "../types.hpp"

// CRTP
struct vertex_t {
protected:
	static arch_t id() noexcept {
		static arch_t i = 0;
		return i++;
	}
};

template<typename V>
struct crtp_vertex_t : public vertex_t {
public:
	static arch_t name() noexcept {
		static arch_t n = vertex_t::id();
		return n;
	}
};

struct vtx_minor_t : public crtp_vertex_t<vtx_minor_t> {
public:
	glm::vec2 position;
	sint_t matrix;
public:
	vtx_minor_t(real_t x, real_t y) :
		position(x, y),
		matrix(0) {}
	vtx_minor_t() :
		position(0.0f),
		matrix(0) {}
};

struct vtx_blank_t : public crtp_vertex_t<vtx_blank_t> {
	glm::vec2 position;
	sint_t matrix;
	glm::vec4 color;
public:
	vtx_blank_t() :
		position(0.0f),
		matrix(0),
		color(1.0f) {}
};

struct vtx_major_t : public crtp_vertex_t<vtx_major_t> {
	glm::vec2 position;
	sint_t matrix;
	glm::vec2 uvcoords;
	real_t table, alpha;
public:
	vtx_major_t() :
		position(0.0f),
		matrix(0),
		uvcoords(0.0f),
		table(0.0f),
		alpha(0.0f) {}
};

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
	static vertex_spec_t from(arch_t vtx);
	bool operator==(const vertex_spec_t& that);
	bool operator!=(const vertex_spec_t& that);
};

#endif // LEVIATHAN_INCLUDED_VIDEO_VERTEX_HPP
