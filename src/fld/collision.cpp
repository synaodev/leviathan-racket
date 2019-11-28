#include "./collision.hpp"
#include "./tilemap.hpp"

#include <limits>

static constexpr sint_t kSintTileSize = 16;
static constexpr real_t kRealTileSize = 16.0f;

collision::info_t::info_t(glm::ivec2 index, sint_t attribute) : 
	index(index),
	attribute(attribute),
	coordinate(0.0f)
{

}

collision::info_t::info_t() :
	index(0),
	attribute(0),
	coordinate(0.0f)
{

}

rect_t collision::info_t::hitbox() const {
	return rect_t(
		static_cast<real_t>(index.x * kSintTileSize),
		static_cast<real_t>(index.y * kSintTileSize),
		kRealTileSize,
		kRealTileSize
	);
}

/*static bool is_slope_tall(const collision::info_t& info) {
	return info.attribute & tile_flag_t::Tall;
}*/

static bool is_slope_opposing_side(const collision::info_t& info, side_t side) {
	if (info.attribute & tile_flag_t::Floor) {
		return side == side_t::Top;
	} else if (info.attribute & tile_flag_t::Ceiling) {
		return side == side_t::Bottom;
	}
	return false;
}

static real_t use_slope_multiplier(const collision::info_t& info) {
	if (info.attribute & tile_flag_t::Positive) {
		return 0.5f;
	}
	return -0.5f;
}

static real_t use_slope_height(const collision::info_t& info) {
	if (info.attribute & tile_flag_t::Ceiling) {
		if (info.attribute & tile_flag_t::Negative) {
			if (info.attribute & tile_flag_t::Tall) {
				return kRealTileSize;
			}
		} else if (info.attribute & tile_flag_t::Positive) {
			if (info.attribute & tile_flag_t::Short) {
				return 0.0f;
			}
		}
	} else if (info.attribute & tile_flag_t::Floor) {
		if (info.attribute & tile_flag_t::Negative) {
			if (info.attribute & tile_flag_t::Short) {
				return kRealTileSize;
			}
		} else if (info.attribute & tile_flag_t::Positive) {
			if (info.attribute & tile_flag_t::Tall) {
				return 0.0f;
			}
		}
	}
	return kRealTileSize / 2.0f;
}

struct collision_result_t {
public:
	collision_result_t() : 
		valid(false),
		coordinate(0.0f) {}
	collision_result_t(real_t coordinate) :
		valid(false),
		coordinate(coordinate) {}
	collision_result_t(const collision_result_t&) = default;
	collision_result_t& operator=(const collision_result_t&) = default;
	collision_result_t(collision_result_t&&) = default;
	collision_result_t& operator=(collision_result_t&&) = default;
	~collision_result_t() = default;
public:
	bool_t valid;
	real_t coordinate;
};

static collision_result_t test_collision(const rect_t& delta, const collision::info_t& info, side_t opposite, real_t perpendicular_position, real_t leading_position, bool should_test_slopes) {
	collision_result_t result = collision_result_t(leading_position);
	if (info.attribute & tile_flag_t::Block) {
		rect_t hitbox = info.hitbox();
		if (delta.overlaps(hitbox)) {
			switch (opposite) {
			case side_t::Right: {
				if (!(info.attribute & tile_flag_t::FallThrough)) {
					result.valid = true;
					result.coordinate = hitbox.right();
				}
				break;
			}
			case side_t::Left: {
				if (!(info.attribute & tile_flag_t::FallThrough)) {
					result.valid = true;
					result.coordinate = hitbox.x;
				}
				break;
			}
			case side_t::Top: {
				if (info.attribute & tile_flag_t::FallThrough) {
					if (delta.bottom() - (kRealTileSize / 2.0f) < hitbox.y) {
						result.valid = true;
						result.coordinate = hitbox.y;
					}
				} else {
					result.valid = true;
					result.coordinate = hitbox.y;
				}
				break;
			}
			default: {
				result.valid = true;
				result.coordinate = hitbox.bottom();
				break;
			}
			}
		}
	} else if (should_test_slopes and (info.attribute & tile_flag_t::Slope) and is_slope_opposing_side(info, opposite)) {
		rect_t hitbox = info.hitbox();
		real_t multiplier = use_slope_multiplier(info);
		real_t height = use_slope_height(info);
		result.coordinate = side_fn::vert(opposite) ?
			multiplier * (perpendicular_position - hitbox.x) + height + hitbox.y :
			(perpendicular_position - hitbox.y - height) / height + hitbox.x;
		result.valid = side_fn::is_max(opposite) ?
			leading_position <= result.coordinate :
			leading_position >= result.coordinate;
	}
	return result;
}

std::optional<collision::info_t> collision::attempt(rect_t delta, std::bitset<phy_t::Total>& flags, const tilemap_t& tilemap, side_t side) {
	sint_t first_primary = tilemap_t::round(delta.side(side_fn::opposing(side)));
	sint_t final_primary = tilemap_t::round(delta.side(side));
	sint_t incrm_primary = side_fn::is_max(side) ? 1 : -1;
	bool horizontal = side_fn::hori(side);
	sint_t s_min = tilemap_t::round(horizontal ? delta.y : delta.x);
	sint_t s_mid = tilemap_t::round(horizontal ? delta.center_y() : delta.center_x());
	sint_t s_max = tilemap_t::round(horizontal ? delta.bottom()	: delta.right());
	bool s_positive = s_mid - s_min < s_max - s_mid;
	sint_t incrm_secondary = s_positive	? 1 : -1;
	sint_t first_secondary = s_positive	? s_min : s_max;
	sint_t final_secondary = !s_positive  ? s_min : s_max;
	for (sint_t primary = first_primary; primary != final_primary + incrm_primary; primary += incrm_primary) {
		for (sint_t secondary = first_secondary; secondary != final_secondary + incrm_secondary; secondary += incrm_secondary) {
			sint_t y = !horizontal ? primary : secondary;
			sint_t x = horizontal ? primary : secondary;
			sint_t a = tilemap.get_attribute(x, y);
			collision::info_t info = collision::info_t(glm::ivec2(x, y), a);
			if (info.attribute & tile_flag_t::OutBounds) {
				return info;
			}
			side_t opposing = side_fn::opposing(side);
			bool should_test_slopes = side_fn::vert(opposing);
			real_t perpendicular_position = side_fn::vert(opposing) ? delta.center_x() : delta.center_y();
			real_t leading_position = delta.side(side);
			collision_result_t result = test_collision(
				delta, info, opposing,
				perpendicular_position,
				leading_position,
				should_test_slopes
			);
			if (result.valid) {
				info.coordinate = result.coordinate;
				return info;
			} else if ((side == side_t::Bottom and flags[phy_t::Bottom]) or (side == side_t::Top and flags[phy_t::Top])) {
				if ((flags[phy_t::Sloped] and info.attribute & tile_flag_t::Slope) or (!flags[phy_t::Sloped] and info.attribute & tile_flag_t::Tall)) {
					info.coordinate = result.coordinate;
					return info;
				}
			}
		}
	}
	return std::nullopt;
}

std::optional<glm::vec2> collision::find_intersection(glm::vec2 ray_pos, glm::vec2 ray_dir, glm::vec2 seg_a, glm::vec2 seg_b) {
	real_t s_dx = seg_b.x - seg_a.x;
	real_t s_dy = seg_b.y - seg_a.y;
	real_t r_mag = std::sqrt(ray_dir.x * ray_dir.x + ray_dir.y * ray_dir.y);
	real_t s_mag = std::sqrt(s_dx * s_dx + s_dy * s_dy);
	if ((ray_dir.x / r_mag == s_dx / s_mag) and (ray_dir.y / r_mag == s_dy / s_mag)) {
		return std::nullopt;
	}
	real_t T2 = (ray_dir.x * (seg_a.y - ray_pos.y) + ray_dir.y * (ray_pos.x - seg_a.x)) / (s_dx * ray_dir.y - s_dy * ray_dir.x);
	real_t T1 = (seg_a.x + s_dx * T2 - ray_pos.x) / ray_dir.x;
	if (T1 < 0.0f) {
		return std::nullopt;
	}
	if (T2 < 0.0f or T2 > 1.0f) {
		return std::nullopt;
	}
	return ray_pos + ray_dir * T1;
}

glm::vec2 collision::trace_ray(const tilemap_t& tilemap, real_t max_length, glm::vec2 origin, glm::vec2 direction) {
	real_t len = 0.0f;
	glm::vec2 index = glm::floor(origin);
	glm::vec2 step(
		direction[0] > 0.0f ? 1.0f : -1.0f,
		direction[1] > 0.0f ? 1.0f : -1.0f
	);
	glm::vec2 deltalen = glm::abs(1.0f / direction);
	glm::vec2 distance(
		step[0] > 0.0f ? index[0] + 1.0f - origin[0] : origin[0] - index[0],
		step[1] > 0.0f ? index[1] + 1.0f - origin[1] : origin[1] - index[1]
	);
	glm::vec2 maxdelta(
		deltalen[0] < std::numeric_limits<real_t>::infinity() ? deltalen[0] * distance[0] : std::numeric_limits<real_t>::infinity(),
		deltalen[1] < std::numeric_limits<real_t>::infinity() ? deltalen[1] * distance[1] : std::numeric_limits<real_t>::infinity()
	);
	while (len <= max_length) {
		glm::length_t I = maxdelta[0] < maxdelta[1] ? 0 : 1;
		index[I] += step[I];
		len = maxdelta[I];
		maxdelta[I] += deltalen[I];
		sint_t xpos = tilemap_t::round(index.x);
		sint_t ypos = tilemap_t::round(index.y);
		sint_t attr = tilemap.get_attribute(xpos, ypos);
		if (attr != tile_flag_t::Empty and !(attr & (tile_flag_t::FallThrough | tile_flag_t::OutBounds))) {
			if (attr & tile_flag_t::Block) {
				if (attr & tile_flag_t::Hooked) {
					return glm::vec2(
						tilemap_t::extend(xpos) + (kRealTileSize / 2.0f),
						tilemap_t::extend(ypos) + (kRealTileSize / 2.0f)
					);
				}
				return origin + len * direction;
			} else if (attr & tile_flag_t::Slope) {
				std::optional<glm::vec2> intersect;
				real_t left = tilemap_t::extend(xpos);
				real_t top = tilemap_t::extend(ypos);
				real_t right = tilemap_t::extend(xpos + 1);
				real_t bottom = tilemap_t::extend(ypos + 1);
				real_t center = top + (kRealTileSize / 2.0f);
				switch (attr) {
				case tile_flag_t::Slope_1:
					intersect = find_intersection(origin, direction, glm::vec2(left, top), glm::vec2(right, center));
					break;
				case tile_flag_t::Slope_2:
					intersect = find_intersection(origin, direction, glm::vec2(left, center), glm::vec2(right, bottom));
					break;
				case tile_flag_t::Slope_3:
					intersect = find_intersection(origin, direction, glm::vec2(left, bottom), glm::vec2(right, center));
					break;
				case tile_flag_t::Slope_4:
					intersect = find_intersection(origin, direction, glm::vec2(left, center), glm::vec2(right, top));
					break;
				case tile_flag_t::Slope_5:
					intersect = find_intersection(origin, direction, glm::vec2(left, bottom), glm::vec2(right, center));
					break;
				case tile_flag_t::Slope_6:
					intersect = find_intersection(origin, direction, glm::vec2(left, center), glm::vec2(right, top));
					break;
				case tile_flag_t::Slope_7:
					intersect = find_intersection(origin, direction, glm::vec2(left, top), glm::vec2(right, center));
					break;
				case tile_flag_t::Slope_8:
					intersect = find_intersection(origin, direction, glm::vec2(left, center), glm::vec2(right, bottom));
					break;
				default:
					break;
				}
				if (intersect.has_value()) {
					return *intersect;
				}
			}
		}
	}
	return origin + len * direction;
}

glm::vec2 collision::trace_ray(const tilemap_t& tilemap, real_t max_length, glm::vec2 origin, real_t angle) { 
	glm::vec2 direction = glm::vec2(glm::cos(angle), glm::sin(angle));
	return trace_ray(
		tilemap,
		max_length,
		origin,
		direction
	);
}