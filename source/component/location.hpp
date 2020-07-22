#ifndef SYNAO_COMPONENT_LOCATION_HPP
#define SYNAO_COMPONENT_LOCATION_HPP

#include "../utility/rect.hpp"
#include "../utility/enums.hpp"

struct renderer_t;
struct kontext_t;

struct location_t {
public:
	location_t();
	location_t(glm::vec2 position);
	location_t(glm::vec2 position, direction_t direction);
	location_t(const location_t&) = default;
	location_t& operator=(const location_t&) = default;
	location_t(location_t&&) = default;
	location_t& operator=(location_t&&) = default;
	~location_t() = default;
public:
	glm::vec2 center() const;
	rect_t hitbox() const;
	bool overlap(const location_t& that) const;
	bool overlap(const rect_t& that) const;
	void hori(direction_t bits);
	void vert(direction_t bits);
public:
	static void render(const kontext_t& kontext, renderer_t& renderer, rect_t viewport);
public:
	glm::vec2 position;
	direction_t direction;
	rect_t bounding;
};

#endif // SYNAO_COMPONENT_LOCATION_HPP