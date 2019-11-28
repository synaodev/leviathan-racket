#include "./kinematics.hpp"
#include "./location.hpp"
#include "./kontext.hpp"

#include "../fld/collision.hpp"

static constexpr real_t kLongFactor  = 2.0f;
static constexpr real_t kShortFactor = 3.0f;

kinematics_t::kinematics_t(glm::vec2 velocity) :
	flags(0),
	velocity(velocity),
	anchor(0.0f),
	tether(0.0f)
{

}

kinematics_t::kinematics_t() :
	flags(0),
	velocity(0.0f),
	anchor(0.0f),
	tether(0.0f)
{

}

void kinematics_t::reset() {
	flags.reset();
	velocity = glm::zero<glm::vec2>();
	anchor = glm::zero<glm::vec2>();
	tether = 0.0f;
}

void kinematics_t::accel_angle(real_t angle, real_t speed) {
	velocity = glm::vec2(
		glm::cos(angle) * speed,
		glm::sin(angle) * speed
	);
}

void kinematics_t::accel_x(real_t amount, real_t limit) {
	velocity.x = amount > 0.0f ? 
		glm::min(velocity.x + amount, limit) :
		glm::max(velocity.x + amount, -limit);
}

void kinematics_t::accel_y(real_t amount, real_t limit) {
	velocity.y = amount > 0.0f ? 
		glm::min(velocity.y + amount, limit) :
		glm::max(velocity.y + amount, -limit);
}

void kinematics_t::decel_x(real_t amount) {
	velocity.x = velocity.x > 0.0f ? 
		glm::max(0.0f, velocity.x - amount) :
		glm::min(0.0f, velocity.x + amount);
}

void kinematics_t::decel_y(real_t amount) {
	velocity.y = velocity.y > 0.0f ? 
		glm::max(0.0f, velocity.y - amount) :
		glm::min(0.0f, velocity.y + amount);
}

bool kinematics_t::hori_sides() const {
	return flags[phy_t::Right] or flags[phy_t::Left];
}

bool kinematics_t::vert_sides() const {
	return flags[phy_t::Top] or flags[phy_t::Bottom];
}

bool kinematics_t::any_side() const {
	return this->hori_sides() or this->vert_sides();
}

void kinematics_t::handle(kontext_t& kontext, const tilemap_t& tilemap) {
	kontext.slice<kinematics_t, location_t>().each([&tilemap](entt::entity, kinematics_t& kinematics, location_t& location) {
		if (kinematics.velocity.x != 0.0f) {
			kinematics_t::do_x(location, kinematics, kinematics.velocity.x, tilemap);
		}
		if (kinematics.velocity.y != 0.0f) {
			kinematics_t::do_y(location, kinematics, kinematics.velocity.y, tilemap);
		}
		if (kinematics.tether > 0.0f) {
			kinematics_t::do_angle(location, kinematics, kinematics.velocity);
		}
	});
}

void kinematics_t::handle(location_t& location, kinematics_t& kinematics, const tilemap_t& tilemap, glm::vec2 inertia) {
	if (inertia.x != 0.0f) {
		kinematics_t::do_x(location, kinematics, inertia.x, tilemap);
	}
	if (inertia.y != 0.0f) {
		kinematics_t::do_y(location, kinematics, inertia.y, tilemap);
	}
	if (kinematics.tether > 0.0f) {
		kinematics_t::do_angle(location, kinematics, inertia);
	}
}

rect_t kinematics_t::predict(const location_t& location, side_t side, real_t inertia) {
	switch (side) {
	case side_t::Left:
		return rect_t(
			location.position.x + location.bounding.x + inertia,
			location.position.y + location.bounding.y + location.bounding.h / kShortFactor,
			location.bounding.w / kLongFactor - inertia,
			location.bounding.h / kShortFactor
		);
	case side_t::Right:
		return rect_t(
			location.position.x + location.bounding.x + location.bounding.w / kLongFactor,
			location.position.y + location.bounding.y + location.bounding.h / kShortFactor,
			location.bounding.w / kLongFactor + inertia,
			location.bounding.h / kShortFactor
		);
	case side_t::Top:
		return rect_t(
			location.position.x + location.bounding.x,
			location.position.y + location.bounding.y + inertia,
			location.bounding.w,
			location.bounding.h / kLongFactor - inertia
		);
	default:
		return rect_t(
			location.position.x + location.bounding.x,
			location.position.y + location.bounding.y + location.bounding.h / kLongFactor,
			location.bounding.w,
			location.bounding.h / kLongFactor + inertia
		);
	}
}

void kinematics_t::do_angle(location_t& location, kinematics_t& kinematics, glm::vec2& inertia) {
	glm::vec2 test_point = location.position + location.bounding.center();
	real_t distance = glm::distance(test_point, kinematics.anchor);
	kinematics.flags[phy_t::Constrained] = distance > kinematics.tether;
	if (kinematics.flags[phy_t::Constrained]) {
		real_t angle = glm::atan(
			kinematics.anchor.y - test_point.y, 
			kinematics.anchor.x - test_point.x
		);
		glm::vec2 normal = glm::vec2(glm::cos(angle), glm::sin(angle));
		test_point += (normal * (distance - kinematics.tether));
		location.position = test_point - location.bounding.center();
		if (inertia != glm::zero<glm::vec2>()) {
			normal = glm::normalize(test_point - kinematics.anchor);
			glm::vec2 perpendicular = glm::vec2(normal.y, -normal.x);
			glm::vec2 redirection = perpendicular * glm::dot(perpendicular, inertia);
			if (redirection != glm::zero<glm::vec2>()) {
				inertia = glm::normalize(redirection) * glm::length(inertia);
			}
		}
	}
}

void kinematics_t::do_x(location_t& location, kinematics_t& kinematics, real_t inertia, const tilemap_t& tilemap) {
	if (!kinematics.flags[phy_t::Noclip]) {
		side_t side = inertia > 0.0f ? side_t::Right : side_t::Left;
		std::optional<collision::info_t> info = collision::attempt(
			kinematics_t::predict(location, side, inertia),
			kinematics.flags,
			tilemap,
			side
		);
		if (info.has_value()) {
			location.position.x = info->coordinate - location.bounding.side(side);
			kinematics.velocity.x = 0.0f;
			kinematics.flags[phy_t::Right] = side == side_t::Right;
			kinematics.flags[phy_t::Left] = side == side_t::Left;
			kinematics.flags[phy_t::Hooked] = info->attribute & tile_flag_t::Hooked;
		} else {
			location.position.x += inertia;
			kinematics.flags[phy_t::Right] = false;
			kinematics.flags[phy_t::Left] = false;
		}
	} else {
		location.position.x += inertia;
		kinematics.flags[phy_t::Right] = false;
		kinematics.flags[phy_t::Left] = false;
	}
}

void kinematics_t::do_y(location_t& location, kinematics_t& kinematics, real_t inertia, const tilemap_t& tilemap) {
	if (!kinematics.flags[phy_t::Noclip]) {
		side_t side = inertia > 0.0f ? side_t::Bottom : side_t::Top;
		std::optional<collision::info_t> info = collision::attempt(
			kinematics_t::predict(location, side, inertia),
			kinematics.flags,
			tilemap,
			side
		);
		if (info.has_value()) {
			if (!(info->attribute & tile_flag_t::OutBounds)) {
				if (info->attribute & tile_flag_t::FallThrough and (side != side_t::Bottom or kinematics.flags[phy_t::WillDrop])) {
					location.position.y += inertia;
					kinematics.flags[phy_t::Top] = false;
					kinematics.flags[phy_t::Bottom] = false;
					kinematics.flags[phy_t::Sloped] = false;
				} else {
					location.position.y = info->coordinate - location.bounding.side(side);
					kinematics.velocity.y = 0.0f;
					kinematics.flags[phy_t::Hooked] = info->attribute & tile_flag_t::Hooked;
					kinematics.flags[phy_t::Sloped] = info->attribute & tile_flag_t::Slope;
					if (side == side_t::Top) {
						kinematics.flags[phy_t::Top] = true;
						kinematics.flags[phy_t::Bottom] = false;
						kinematics.flags[phy_t::FallThrough] = false;
					} else {
						kinematics.flags[phy_t::Top] = false;
						kinematics.flags[phy_t::Bottom] = true;
						kinematics.flags[phy_t::FallThrough] = info->attribute & tile_flag_t::FallThrough;
					}
				}
			} else {
				location.position.y += inertia;
				kinematics.flags[phy_t::Outbounds] = true;
				kinematics.flags[phy_t::Noclip] = true;
			}
		} else {
			location.position.y += inertia;
			kinematics.flags[phy_t::Top] = false;
			kinematics.flags[phy_t::Bottom] = false;
			kinematics.flags[phy_t::Sloped] = false;
			kinematics.flags[phy_t::WillDrop] = false;
		}
	} else {
		location.position.y += inertia;
		kinematics.flags[phy_t::Top] = false;
		kinematics.flags[phy_t::Bottom] = false;
		kinematics.flags[phy_t::Sloped] = false;
		kinematics.flags[phy_t::WillDrop] = false;
	}
}
