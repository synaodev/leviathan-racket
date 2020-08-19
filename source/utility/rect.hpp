#ifndef LEVIATHAN_INCLUDED_UTILITY_RECT_HPP
#define LEVIATHAN_INCLUDED_UTILITY_RECT_HPP

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
	real_t x, y, w, h;
public:
	rect_t(glm::vec2 pos, glm::vec2 dim);
	rect_t(glm::vec2 dim);
	rect_t(real_t x, real_t y, real_t w, real_t h);
	rect_t();
	rect_t(const rect_t& that) noexcept;
	rect_t(rect_t&& that) noexcept;
	rect_t& operator=(const rect_t& that) noexcept;
	rect_t& operator=(rect_t&& that) noexcept;
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
	inline glm::vec2 left_top() const { return glm::vec2(x, y); }
	inline glm::vec2 left_bottom() const { return glm::vec2(x, y + h); }
	inline glm::vec2 right_top() const { return glm::vec2(x + w, y); }
	inline glm::vec2 right_bottom() const { return glm::vec2(x + w, y + h); }
	inline glm::vec2 dimensions() const { return glm::vec2(w, h); }
	inline glm::vec2 center() const { return glm::vec2(x + (w / 2.0f), y + (h / 2.0f)); }
	inline real_t center_x() const { return x + (w / 2.0f); }
	inline real_t center_y() const { return y + (h / 2.0f); }
	void push_fix(rect_t* that) const;
	void push_fix(glm::vec2* mid, glm::vec2 dim) const;
	bool contains(glm::vec2 v) const;
	bool contains(const rect_t& that) const;
	bool overlaps(const rect_t& that) const;
	bool overlaps(glm::vec2 that_pos, glm::vec2 that_dim) const;
	bool intersects(glm::vec2 ray_pos, glm::vec2 ray_dir) const;
	bool intersects(const rect_t& that, rect_t* intersection) const;
	bool cmp_round(const rect_t& that) const;
	bool operator==(const rect_t& that) const;
	bool operator!=(const rect_t& that) const;
	bool operator==(const glm::vec4& that) const;
	bool operator!=(const glm::vec4& that) const;
};

#endif // LEVIATHAN_INCLUDED_UTILITY_RECT_HPP
