#ifndef LEVIATHAN_INCLUDED_VIDEO_ANIMATION_HPP
#define LEVIATHAN_INCLUDED_VIDEO_ANIMATION_HPP

#include <vector>
#include <string>
#include <atomic>
#include <future>

#include "../utility/rect.hpp"
#include "../utility/enums.hpp"

struct thread_pool_t;
struct texture_t;
struct palette_t;
struct renderer_t;

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

struct animation_t : public not_copyable_t {
public:
	animation_t();
	animation_t(animation_t&& that) noexcept;
	animation_t& operator=(animation_t&& that) noexcept;
	~animation_t() = default;
public:
	void update(real64_t delta, bool_t& amend, arch_t state, real64_t& timer, arch_t& frame) const;
	void render(renderer_t& renderer, const rect_t& viewport, bool_t panic, bool_t& amend, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, real_t index, glm::vec2 position, glm::vec2 scale, real_t angle, glm::vec2 pivot) const;
	void render(renderer_t& renderer, const rect_t& viewport, bool_t panic, bool_t& amend, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, real_t index, glm::vec2 position, glm::vec2 scale) const;
	void render(renderer_t& renderer, bool_t& amend, arch_t state, arch_t frame, arch_t variation, real_t index, glm::vec2 position) const;
	void load(const std::string& full_path);
	void load(const std::string& full_path, thread_pool_t& thread_pool);
	void assure() const;
	bool visible(const rect_t& viewport, arch_t state, arch_t frame, arch_t variation, layer_t layer, glm::vec2 position, glm::vec2 scale) const;
	bool is_finished(arch_t state, arch_t frame, real64_t timer) const;
	glm::vec2 get_origin(arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring) const;
	glm::vec2 get_action_point(arch_t state, arch_t variation, mirroring_t mirroring) const;
private:
	std::atomic<bool> ready;
	std::future<void> future;
	std::vector<animation_sequence_t> sequences;
	glm::vec2 inverts;
	const texture_t* texture;
	const palette_t* palette;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_ANIMATION_HPP
