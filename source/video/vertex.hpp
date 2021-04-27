#ifndef LEVIATHAN_INCLUDED_VIDEO_VERTEX_HPP
#define LEVIATHAN_INCLUDED_VIDEO_VERTEX_HPP

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

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
	vtx_minor_t() = default;
	vtx_minor_t(const vtx_minor_t&) = default;
	vtx_minor_t(vtx_minor_t&&) noexcept = default;
	vtx_minor_t& operator=(const vtx_minor_t&) = default;
	vtx_minor_t& operator=(vtx_minor_t&&) noexcept = default;
	~vtx_minor_t() = default;
public:
	glm::vec2 position {};
};

struct vtx_blank_t : public crtp_vertex_t<vtx_blank_t> {
public:
	vtx_blank_t() = default;
	vtx_blank_t(const vtx_blank_t&) = default;
	vtx_blank_t(vtx_blank_t&&) noexcept = default;
	vtx_blank_t& operator=(const vtx_blank_t&) = default;
	vtx_blank_t& operator=(vtx_blank_t&&) noexcept = default;
	~vtx_blank_t() = default;
public:
	glm::vec2 position {};
	sint_t matrix { 0 };
	glm::vec4 color { 1.0f };
};

struct vtx_major_t : public crtp_vertex_t<vtx_major_t> {
public:
	vtx_major_t() = default;
	vtx_major_t(const vtx_major_t&) = default;
	vtx_major_t(vtx_major_t&&) noexcept = default;
	vtx_major_t& operator=(const vtx_major_t&) = default;
	vtx_major_t& operator=(vtx_major_t&&) noexcept = default;
	~vtx_major_t() = default;
public:
	glm::vec2 position {};
	sint_t matrix { 0 };
	glm::vec2 uvcoords {};
	real_t alpha { 0.0f };
	sint_t texID { 0 };
	sint_t palID { 0 };
};

struct vtx_fonts_t : public crtp_vertex_t<vtx_fonts_t> {
public:
	vtx_fonts_t() = default;
	vtx_fonts_t(const vtx_fonts_t&) = default;
	vtx_fonts_t(vtx_fonts_t&&) noexcept = default;
	vtx_fonts_t& operator=(const vtx_fonts_t&) = default;
	vtx_fonts_t& operator=(vtx_fonts_t&&) noexcept = default;
	~vtx_fonts_t() = default;
public:
	glm::vec2 position {};
	glm::vec2 uvcoords {};
	glm::vec4 color { 1.0f };
	sint_t atlas { 0 };
	sint_t table { 0 };
};

struct vertex_spec_t {
public:
	void(*detail)(void) { nullptr };
	arch_t length { 0 };
public:
	vertex_spec_t() = default;
	vertex_spec_t(const vertex_spec_t&) = default;
	vertex_spec_t(vertex_spec_t&&) noexcept = default;
	vertex_spec_t& operator=(const vertex_spec_t&) = default;
	vertex_spec_t& operator=(vertex_spec_t&&) noexcept = default;
	~vertex_spec_t() = default;
public:
	static bool compare(const uint_t* lhv, const uint_t* rhv);
	static vertex_spec_t from(const uint_t* list);
	static vertex_spec_t from(arch_t vtx);
	bool operator==(const vertex_spec_t& that) {
		return (
			this->detail == that.detail and
			this->length == that.length
		);
	}
	bool operator!=(const vertex_spec_t& that) {
		return !(*this == that);
	}
};

#endif // LEVIATHAN_INCLUDED_VIDEO_VERTEX_HPP
