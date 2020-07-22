#ifndef SYNAO_SYSTEM_CAMERA_HPP
#define SYNAO_SYSTEM_CAMERA_HPP

#include <glm/mat4x4.hpp>

#include "../utility/rect.hpp"

struct naomi_state_t;

struct camera_t : public not_copyable_t {
public:
	camera_t();
	camera_t(camera_t&&) = default;
	camera_t& operator=(camera_t&&) = default;
	~camera_t() = default;
public:
	void reset();
	void handle(const naomi_state_t& naomi_state);
	void update(real64_t delta);
	void set_view_limits(rect_t view_limits);
	void set_focus(glm::vec2 position);
	void quake(real_t power, real64_t seconds);
	void quake(real_t power);
	rect_t get_viewport() const;
	arch_t get_tile_range(glm::ivec2 first, glm::ivec2 last) const;
	glm::mat4 get_matrix() const;
private:
	bool_t cycling, indefinite;
	real64_t timer;
	rect_t view_limits;
	glm::vec2 position, dimensions, offsets;
	real_t quake_power, view_angle;
};

#endif // SYNAO_SYSTEM_CAMERA_HPP