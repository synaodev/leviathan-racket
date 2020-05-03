#ifndef SYNAO_VIDEO_ANIMATION_HPP
#define SYNAO_VIDEO_ANIMATION_HPP

#include <future>

#include "../utility/enums.hpp"

#include "./animation_sequence.hpp"

struct thread_pool_t;
struct texture_t;
struct palette_t;
struct renderer_t;

struct animation_t : public not_copyable_t {
public:
	animation_t();
	animation_t(animation_t&& that) noexcept;
	animation_t& operator=(animation_t&& that) noexcept;
	~animation_t() = default;
public:
	void update(real64_t delta, bool_t& write, arch_t state, real64_t& timer, arch_t& frame) const;
	void render(renderer_t& renderer, const rect_t& viewport, bool_t panic, bool_t& write, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, real_t index, glm::vec2 position, glm::vec2 scale, real_t angle, glm::vec2 pivot) const;
	void render(renderer_t& renderer, const rect_t& viewport, bool_t panic, bool_t& write, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, real_t index, glm::vec2 position, glm::vec2 scale) const;
	void render(renderer_t& renderer, bool_t& write, arch_t state, arch_t frame, arch_t variation, real_t index, glm::vec2 position) const;
	bool load(const std::string& full_path);
	bool load(const std::string& full_path, thread_pool_t& thread_pool);
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

#endif // SYNAO_VIDEO_ANIMATION_HPP