#include "./animation_sequence.hpp"

animation_sequence_t::animation_sequence_t() :
	frames(),
	action_points(),
	dimensions(0.0f),
	delay(0.0),
	total(0),
	repeat(true)
{

}

animation_sequence_t::animation_sequence_t(glm::vec2 dimensions, real64_t delay, arch_t total, bool_t repeat) :
	frames(),
	action_points(),
	dimensions(dimensions),
	delay(delay),
	total(total),
	repeat(repeat)
{

}

animation_sequence_t::animation_sequence_t(animation_sequence_t&& that) noexcept : animation_sequence_t() {
	if (this != &that) {
		std::swap(frames, that.frames);
		std::swap(action_points, that.action_points);
		std::swap(dimensions, that.dimensions);
		std::swap(delay, that.delay);
		std::swap(total, that.total);
		std::swap(repeat, that.repeat);
	}
}

animation_sequence_t& animation_sequence_t::operator=(animation_sequence_t&& that) noexcept {
	if (this != &that) {
		std::swap(frames, that.frames);
		std::swap(action_points, that.action_points);
		std::swap(dimensions, that.dimensions);
		std::swap(delay, that.delay);
		std::swap(total, that.total);
		std::swap(repeat, that.repeat);
	}
	return *this;
}

void animation_sequence_t::append(glm::vec2 action_point) {
	action_points.push_back(action_point);
}

void animation_sequence_t::append(glm::vec2 invert, glm::vec2 start, glm::vec4 points) {
	const glm::vec2 position = invert * (start + (glm::vec2(points[0], points[1]) * dimensions));
	const glm::vec2 origin = glm::vec2(points[2], points[3]);
	frames.emplace_back(position, origin);
}

const sequence_frame_t& animation_sequence_t::get_frame(arch_t frame, direction_t direction) const {
	static const sequence_frame_t kFrameZero = sequence_frame_t(
		glm::zero<glm::vec2>(),
		glm::one<glm::vec2>()
	);
	arch_t index = frame + (direction * total);
	if (index < frames.size()) {
		return frames[index];
	}
	return kFrameZero;
}

rect_t animation_sequence_t::get_quad(glm::vec2 invert, arch_t frame, direction_t direction) const {
	arch_t index = frame + (direction * total);
	if (index < frames.size()) {
		return rect_t(frames[index].position, dimensions * invert);
	}
	return rect_t(
		glm::zero<glm::vec2>(), 
		glm::one<glm::vec2>()
	);
}

glm::vec2 animation_sequence_t::get_dimensions() const {
	return dimensions;
}

glm::vec2 animation_sequence_t::get_origin(arch_t frame, direction_t direction) const {
	arch_t index = frame + (direction * total);
	if (index < frames.size()) {
		return frames[index].origin;
	}
	return glm::zero<glm::vec2>();
}

glm::vec2 animation_sequence_t::get_action_point(direction_t direction) const {
	if (direction < action_points.size()) {
		return action_points[direction];
	}
	return glm::zero<glm::vec2>();
}

void animation_sequence_t::update(real64_t delta, bool_t& write, real64_t& timer, arch_t& frame) const {
	if (total > 1) {
		if (repeat) {
			timer += delta;
			if (timer >= delay) {
				write = true;
				timer = glm::mod(timer, delay);
				frame++;
				frame %= total;
			}
		} else {
			if (frame != total - 1) {
				timer += delta;
				if (timer >= delay) {
					write = true;
					timer = glm::mod(timer, delay);
					frame++;
				}
			} else if (timer <= delay) {
				timer += delta;
			}
		}
	} else {
		timer = 0.0f;
		frame = 0;
	}
}

bool animation_sequence_t::is_finished(arch_t frame, real64_t timer) const {
	if (frame == total - 1) {
		return timer > delay;
	}
	return false;
}