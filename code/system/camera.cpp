#include "./camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "../video/quad_batch.hpp"
#include "../actor/naomi.hpp"

static const glm::vec2 kDefaultSpeed  = glm::vec2(16.0f, 8.0f);
static const glm::vec2 kDefaultSizes  = glm::vec2(320.0f, 180.0f);
static const glm::vec2 kDefaultLowest = glm::vec2(8.0f, 6.0f);
static const glm::vec2 kDefaultCenter = (kDefaultSizes / 2.0f) + kDefaultLowest;

camera_t::camera_t() :
	cycling(false),
	indefinite(false),
	timer(0.0),
	view_limits(kDefaultLowest, kDefaultSizes),
	position(kDefaultCenter),
	dimensions(kDefaultSizes),
	offsets(0.0f),
	quake_power(0.0f),
	view_angle(0.0f)
{

}

void camera_t::reset() {
	cycling = false;
	indefinite = false;
	timer = 0.0;
	view_limits = rect_t(kDefaultLowest, kDefaultSizes);
	position = kDefaultCenter;
	dimensions = kDefaultSizes;
	offsets = glm::zero<glm::vec2>();
	quake_power = 0.0f;
	view_angle = 0.0f;
}

void camera_t::handle(const naomi_state_t& naomi_state) {
	glm::vec2 center = naomi_state.camera_placement();
	position += (center - position) / kDefaultSpeed;
	view_limits.push_fix(&position, dimensions);
}

void camera_t::update(real64_t delta) {
	if (quake_power != 0.0) {
		if (cycling) {
			cycling = false;
			offsets = -offsets;
			view_angle = 0.0f;
		} else {
			cycling = true;
			offsets = glm::vec2(
				rng::next(-quake_power, quake_power),
				rng::next(-quake_power, quake_power)
			);
			view_angle = glm::radians(
				rng::next(-quake_power, quake_power)
			);
		}
		if (!indefinite) {
			timer -= delta;
			if (timer <= 0.0) {
				cycling = false;
				timer = 0.0;
				offsets = glm::zero<glm::vec2>();
				quake_power = 0.0f;
				view_angle = 0.0f;
			}
		}
	}
}

void camera_t::set_view_limits(rect_t view_limits) {
	this->view_limits.w = view_limits.w - 16.0f;
	this->view_limits.h = view_limits.h - 12.0f;
	position = kDefaultCenter;
	dimensions = kDefaultSizes;
}

void camera_t::set_focus(glm::vec2 position) {
	view_limits.push_fix(&position, dimensions);
	this->position = position;
}

void camera_t::quake(real_t power, real64_t seconds) {
	if (!indefinite) {
		cycling = false;
		timer = seconds;
		quake_power = power;
		view_angle = 0.0f;
	}
}

void camera_t::quake(real_t power) {
	if (!indefinite) {
		cycling = false;
		indefinite = false;
		timer = 0.0;
		offsets = glm::zero<glm::vec2>();
		quake_power = power;
		view_angle = 0.0f;
	} else {
		cycling = false;
		indefinite = true;
		timer = 0.0;
		quake_power = 0.0f;
		view_angle = 0.0f;
	}
}

rect_t camera_t::get_viewport() const {
	return rect_t(
		position - (dimensions / 2.0f),
		dimensions
	);
}

arch_t camera_t::get_tile_range(glm::ivec2 first, glm::ivec2 last) const {
	glm::ivec2 result = last - first;
	return static_cast<arch_t>(result.x) * static_cast<arch_t>(result.y) * quad_batch_t::SingleQuad;
}

glm::mat4 camera_t::get_matrix() const {
	glm::vec2 result = glm::round(position + offsets);
	glm::mat4 matrix = glm::mat4(1.0f);
	matrix = glm::scale(matrix, glm::vec3(2.0f / dimensions.x, 2.0f / -dimensions.y, 0.0f));
	if (view_angle != 0.0f) {
		matrix = glm::rotate(matrix, view_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	matrix = glm::translate(matrix, -glm::vec3(result, 0.0f));
	return matrix;
}
