#pragma once

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
	glm::vec2 position {};
	glm::vec2 origin {};
public:
	sequence_frame_t(const glm::vec2& position, const glm::vec2& origin) :
		position(position),
		origin(origin) {}
	sequence_frame_t() = default;
	sequence_frame_t(const sequence_frame_t&) = default;
	sequence_frame_t(sequence_frame_t&& that) = default;
	sequence_frame_t& operator=(const sequence_frame_t&) noexcept = default;
	sequence_frame_t& operator=(sequence_frame_t&&) noexcept = default;
	~sequence_frame_t() = default;
};

struct animation_sequence_t : public not_copyable_t {
public:
	animation_sequence_t() = default;
	animation_sequence_t(const glm::vec2& dimensions, real64_t delay, arch_t total, bool_t repeat, bool_t reflect) :
		frames(),
		action_points(),
		dimensions(dimensions),
		delay(delay),
		total(total),
		repeat(repeat),
		reflect(reflect) {}
	animation_sequence_t(animation_sequence_t&& that) noexcept = default;
	animation_sequence_t& operator=(animation_sequence_t&& that) noexcept = default;
	~animation_sequence_t() = default;
public:
	void append(const glm::vec2& action_point);
	void append(const glm::vec2& invert, const glm::vec2& start, const glm::vec4& points);
	void update(real64_t delta, real64_t& timer, arch_t& frame) const;
	void update(real64_t delta, bool_t& amend, real64_t& timer, arch_t& frame) const;
	const sequence_frame_t& get_frame(arch_t frame, arch_t variation) const;
	rect_t get_quad(const glm::vec2& invert, arch_t frame, arch_t variation) const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_origin(arch_t frame, arch_t variation, mirroring_t mirroring) const;
	glm::vec2 get_action_point(arch_t variation, mirroring_t mirroring) const;
	bool is_finished(arch_t frame, real64_t timer) const;
private:
	std::vector<sequence_frame_t> frames {};
	std::vector<glm::vec2> action_points {};
	glm::vec2 dimensions {};
	real64_t delay { 0.0 };
	arch_t total { 0 };
	bool_t repeat { true };
	bool_t reflect { false };
};

struct animation_t : public not_copyable_t, public not_moveable_t {
public:
	animation_t() = default;
	~animation_t();
public:
	void update(real64_t delta, bool_t& amend, arch_t state, real64_t& timer, arch_t& frame) const;
	void update(real64_t delta, arch_t state, real64_t& timer, arch_t& frame) const;
	void render(renderer_t& renderer, const rect_t& viewport, /* bool_t panic, bool_t& amend,*/ arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, sint_t table, glm::vec2 position, glm::vec2 scale, real_t angle, glm::vec2 pivot) const;
	void render(renderer_t& renderer, const rect_t& viewport, /* bool_t panic, bool_t& amend,*/ arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, sint_t table, glm::vec2 position, glm::vec2 scale) const;
	void render(renderer_t& renderer, bool_t& amend, arch_t state, arch_t frame, arch_t variation, sint_t table, glm::vec2 position) const;
	void load(const std::string& full_path);
	void load(const std::string& full_path, thread_pool_t& thread_pool);
	void assure() const;
	// bool visible(const rect_t& viewport, arch_t state, arch_t frame, arch_t variation, layer_t layer, glm::vec2 position, glm::vec2 scale) const;
	bool is_finished(arch_t state, arch_t frame, real64_t timer) const;
	glm::vec2 get_origin(arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring) const;
	glm::vec2 get_action_point(arch_t state, arch_t variation, mirroring_t mirroring) const;
private:
	std::atomic<bool> ready { false };
	std::future<void> future {};
	std::vector<animation_sequence_t> sequences {};
	glm::vec2 inverts { 1.0f };
	const texture_t* texture { nullptr };
	const palette_t* palette { nullptr };
};
