#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "../types.hpp"

namespace __enum_side {
	enum type : arch_t {
		Right,
		Left,
		Top,
		Bottom
	};
}

using side_t = __enum_side::type;

namespace side_fn {
	inline side_t opposing(side_t s) {
		switch (s) {
		case side_t::Right:
			return side_t::Left;
		case side_t::Left:
			return side_t::Right;
		case side_t::Top:
			return side_t::Bottom;
		default:
			return side_t::Top;
		}
	}
	inline bool vert(side_t s) {
		return s == side_t::Top or s == side_t::Bottom;
	}
	inline bool hori(side_t s) {
		return !vert(s);
	}
	inline bool is_max(side_t s) {
		return s == side_t::Right or s == side_t::Bottom;
	}
	inline bool is_min(side_t s) {
		return !is_max(s);
	}
}

struct rect_t {
public:
	real_t x { 0.0f };
	real_t y { 0.0f };
	real_t w { 0.0f };
	real_t h { 0.0f };
public:
	rect_t() = default;
	rect_t(const glm::vec2& p, const glm::vec2& d) :
		x(p.x), y(p.y), w(d.x), h(d.y) {}
	rect_t(const glm::vec2& d) :
		x(0.0f), y(0.0f), w(d.x), h(d.y) {}
	rect_t(real_t x, real_t y, real_t w, real_t h) :
		x(x), y(y), w(w), h(h) {}
	rect_t(const rect_t&) = default;
	rect_t& operator=(const rect_t&) = default;
	rect_t(rect_t&&) noexcept = default;
	rect_t& operator=(rect_t&&) noexcept = default;
	~rect_t() = default;
public:
	inline real_t side(side_t s) const {
		switch (s) {
		case side_t::Right:
			return x + w;
		case side_t::Left:
			return x;
		case side_t::Top:
			return y;
		default:
			return y + h;
		}
	}
	inline real_t right() const { return x + w; }
	inline real_t bottom() const { return y + h; }
	inline glm::vec2 left_top() const { return { x, y }; }
	inline glm::vec2 left_bottom() const { return { x, y + h }; }
	inline glm::vec2 right_top() const { return { x + w, y }; }
	inline glm::vec2 right_bottom() const { return { x + w, y + h }; }
	inline glm::vec2 dimensions() const { return { w, h }; }
	inline glm::vec2 center() const { return { x + (w / 2.0f), y + (h / 2.0f) }; }
	inline real_t center_x() const { return x + (w / 2.0f); }
	inline real_t center_y() const { return y + (h / 2.0f); }
	void push_fix(rect_t& that) const {
		if (that.x < this->x) {
			that.x = this->x;
		}
		if (that.y < this->y) {
			that.y = this->y;
		}
		if (that.right() > this->right()) {
			that.x = this->right() - that.w;
		}
		if (that.bottom() > this->bottom()) {
			that.y = this->bottom() - that.h;
		}
	}
	void push_fix(glm::vec2& m, const glm::vec2& d) const {
		glm::vec2 p = m - (d / 2.0f);
		if (p.x < this->x) {
			p.x = this->x;
		}
		if (p.y < this->y) {
			p.y = this->y;
		}
		if ((p.x + d.x) > this->right()) {
			p.x = this->right() - d.x;
		}
		if ((p.y + d.y) > this->bottom()) {
			p.y = this->bottom() - d.y;
		}
		m = p + (d / 2.0f);
	}
	bool contains(const glm::vec2& p) const {
		return
			this->right() >= p.x and
			this->x <= p.x and
			this->y <= p.y and
			this->bottom() >= p.y;
	}
	bool contains(const rect_t& that) const {
		return
			this->right() >= that.right() and
			this->x <= that.x and
			this->y <= that.y and
			this->bottom() >= that.bottom();
	}
	bool overlaps(const rect_t& that) const {
		return
			this->right() > that.x and
			this->x < that.right() and
			this->y < that.bottom() and
			this->bottom() > that.y;
	}
	bool overlaps(const glm::vec2& p, const glm::vec2& d) const {
		const rect_t that { p, d };
		return this->overlaps(that);
	}
	bool round_compare(const rect_t& that) const {
		const glm::ivec4 lhv { this->x, this->y, this->w, this->h };
		const glm::ivec4 rhv { that.x, that.y, that.w, that.h };
		return lhv == rhv;
	}
	bool operator==(const rect_t& that) const {
		return
			this->x != that.x or
			this->y != that.y or
			this->w != that.w or
			this->h != that.h;
	}
	bool operator!=(const rect_t& that) const {
		return !(*this == that);
	}
	bool operator==(const glm::vec4& that) const {
		return
			this->x != that.x or
			this->y != that.y or
			this->w != that.z or
			this->h != that.w;
	}
	bool operator!=(const glm::vec4& that) const {
		return !(*this == that);
	}
};
