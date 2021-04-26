#ifndef LEVIATHAN_INCLUDED_COMPONENT_LOCATION_HPP
#define LEVIATHAN_INCLUDED_COMPONENT_LOCATION_HPP

#include "../utility/rect.hpp"
#include "../utility/enums.hpp"

struct renderer_t;
struct kontext_t;

struct location_t {
public:
	location_t(const glm::vec2& position) :
		position(position) {}
	location_t(const glm::vec2& position, direction_t direction) :
		position(position),
		direction(direction) {}
	location_t() = default;
	location_t(const location_t&) = default;
	location_t& operator=(const location_t&) = default;
	location_t(location_t&&) noexcept = default;
	location_t& operator=(location_t&&) noexcept = default;
	~location_t() = default;
public:
	glm::vec2 center() const;
	rect_t hitbox() const;
	bool overlap(const location_t& that) const;
	bool overlap(const rect_t& that) const;
	void hori(direction_t bits);
	void vert(direction_t bits);
public:
	static void render(const kontext_t& kontext, renderer_t& renderer, const rect_t& viewport);
public:
	glm::vec2 position {};
	direction_t direction { direction_t::Right };
	rect_t bounding {
		0.0f, 0.0f, 16.0f, 16.0f
	};
};

#endif // LEVIATHAN_INCLUDED_COMPONENT_LOCATION_HPP
