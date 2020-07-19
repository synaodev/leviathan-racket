#ifndef SYNAO_VIDEO_ANIMATION_SEQUENCE_HPP
#define SYNAO_VIDEO_ANIMATION_SEQUENCE_HPP

#include <vector>
#include <string>

#include <array>
#include <vector>
#include <string>

#include "../utility/enums.hpp"
#include "../utility/rect.hpp"

struct sequence_frame_t {
public:
	glm::vec2 position, origin;
public:
	sequence_frame_t(glm::vec2 position, glm::vec2 origin) : 
		position(position), 
		origin(origin) {}
	sequence_frame_t() = default;
	sequence_frame_t(const sequence_frame_t&) = default;
	sequence_frame_t(sequence_frame_t&& that) = default;
	sequence_frame_t& operator=(const sequence_frame_t&) = default;
	sequence_frame_t& operator=(sequence_frame_t&&) = default;
	~sequence_frame_t() = default;
};

struct animation_sequence_t : public not_copyable_t {
public:
	animation_sequence_t();
	animation_sequence_t(glm::vec2 dimensions, real64_t delay, arch_t total, bool_t repeat, bool_t reflect);
	animation_sequence_t(animation_sequence_t&& that) noexcept;
	animation_sequence_t& operator=(animation_sequence_t&& that) noexcept;
	~animation_sequence_t() = default;
public:
	void append(glm::vec2 axnpnt);
	void append(glm::vec2 invert, glm::vec2 start, glm::vec4 points);
	void update(real64_t delta, bool_t& amend, real64_t& timer, arch_t& frame) const;
	const sequence_frame_t& get_frame(arch_t frame, arch_t variation) const;
	rect_t get_quad(glm::vec2 invert, arch_t frame, arch_t variation) const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_origin(arch_t frame, arch_t variation, mirroring_t mirroring) const;
	glm::vec2 get_action_point(arch_t variation, mirroring_t mirroring) const;
	bool is_finished(arch_t frame, real64_t timer) const;
private:
	std::vector<sequence_frame_t> frames;
	std::vector<glm::vec2> action_points;
	glm::vec2 dimensions;
	real64_t delay;
	arch_t total;
	bool_t repeat, reflect;
};

#endif // SYNAO_VIDEO_ANIMATION_SEQUENCE_HPP