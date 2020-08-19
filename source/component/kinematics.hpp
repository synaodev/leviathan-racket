#ifndef LEVIATHAN_INCLUDED_COMPONENT_KINEMATICS_HPP
#define LEVIATHAN_INCLUDED_COMPONENT_KINEMATICS_HPP

#include "./common.hpp"

#include "../utility/rect.hpp"

struct tilemap_t;
struct location_t;
struct kontext_t;

struct kinematics_t {
public:
	kinematics_t(glm::vec2 velocity);
	kinematics_t();
	kinematics_t(const kinematics_t&) = default;
	kinematics_t& operator=(const kinematics_t&) = default;
	kinematics_t(kinematics_t&&) = default;
	kinematics_t& operator=(kinematics_t&&) = default;
	~kinematics_t() = default;
public:
	void reset();
	void accel_angle(real_t angle, real_t speed);
	void accel_x(real_t speed, real_t limit);
	void accel_y(real_t speed, real_t limit);
	void decel_x(real_t speed);
	void decel_y(real_t speed);
	bool hori_sides() const;
	bool vert_sides() const;
	bool any_side() const;
public:
	static void handle(kontext_t& kontext, const tilemap_t& tilemap);
	static void handle(location_t& location, kinematics_t& kinematics, const tilemap_t& tilemap, glm::vec2 inertia);
	static rect_t predict(const location_t& location, side_t side, real_t inertia);
private:
	static void do_angle(location_t& location, kinematics_t& kinematics, glm::vec2& inertia);
	static void do_x(location_t& location, kinematics_t& kinematics, real_t inertia, const tilemap_t& tilemap);
	static void do_y(location_t& location, kinematics_t& kinematics, real_t inertia, const tilemap_t& tilemap);
public:
	std::bitset<phy_t::Total> flags;
	glm::vec2 velocity, anchor;
	real_t tether;
};

#endif // LEVIATHAN_INCLUDED_COMPONENT_KINEMATICS_HPP
