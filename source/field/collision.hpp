#pragma once

#include <bitset>
#include <optional>
#include <glm/vec2.hpp>

#include "./tileflag.hpp"

#include "../component/common.hpp"
#include "../utility/rect.hpp"

struct tilemap_t;

namespace collision {
	struct info_t {
	public:
		info_t(const glm::ivec2& index, uint_t attribute) :
			index(index),
			attribute(attribute) {}
		info_t() = default;
		info_t(const info_t&) = default;
		info_t& operator=(const info_t&) = default;
		info_t(info_t&&) noexcept = default;
		info_t& operator=(info_t&&) noexcept = default;
		~info_t() = default;
	public:
		rect_t hitbox() const;
	public:
		glm::ivec2 index {};
		uint_t attribute { 0 };
		real_t coordinate { 0.0f };
	};
	std::optional<info_t> attempt(rect_t delta, std::bitset<phy_t::Total>& flags, const tilemap_t& tilemap, side_t side);
	std::optional<glm::vec2> find_intersection(glm::vec2 ray_pos, glm::vec2 ray_dir, glm::vec2 seg_a, glm::vec2 seg_b);
	glm::vec2 trace_ray(const tilemap_t& tilemap, real_t max_length, glm::vec2 origin, glm::vec2 direction);
	glm::vec2 trace_ray(const tilemap_t& tilemap, real_t max_length, glm::vec2 origin, real_t angle);
}
