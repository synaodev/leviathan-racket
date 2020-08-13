#ifndef SYNAO_FIELD_COLLISION_HPP
#define SYNAO_FIELD_COLLISION_HPP

#include <bitset>
#include <optional>

#include "./tileflag.hpp"

#include "../utility/rect.hpp"
#include "../component/common.hpp"

struct tilemap_t;

namespace collision {
	struct info_t {
	public:
		info_t(glm::ivec2 index, sint_t attribute);
		info_t();
		info_t(const info_t&) = default;
		info_t& operator=(const info_t&) = default;
		info_t(info_t&&) = default;
		info_t& operator=(info_t&&) = default;
		~info_t() = default;
	public:
		rect_t hitbox() const;
	public:
		glm::ivec2 index;
		sint_t attribute;
		real_t coordinate;
	};
	std::optional<info_t> attempt(rect_t delta, std::bitset<phy_t::Total>& flags, const tilemap_t& tilemap, side_t side);
	std::optional<glm::vec2> find_intersection(glm::vec2 ray_pos, glm::vec2 ray_dir, glm::vec2 seg_a, glm::vec2 seg_b);
	glm::vec2 trace_ray(const tilemap_t& tilemap, real_t max_length, glm::vec2 origin, glm::vec2 direction);
	glm::vec2 trace_ray(const tilemap_t& tilemap, real_t max_length, glm::vec2 origin, real_t angle);
}

#endif // SYNAO_FIELD_COLLISION_HPP