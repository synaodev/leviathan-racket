#pragma once

#include "../overlay/draw-count.hpp"
#include "../overlay/draw-scheme.hpp"

struct input_t;
struct audio_t;
struct kernel_t;
struct receiver_t;
struct stack_gui_t;
struct dialogue_gui_t;
struct headsup_gui_t;

struct inventory_gui_element_t : public not_copyable_t {
public:
	inventory_gui_element_t() = default;
	inventory_gui_element_t(inventory_gui_element_t&&) noexcept = default;
	inventory_gui_element_t& operator=(inventory_gui_element_t&&) noexcept = default;
	~inventory_gui_element_t() = default;
public:
	void init(const texture_t* texture, const animation_t* animation, arch_t index);
	void reset(const glm::ivec4& item);
	void update(real64_t delta);
	void render(renderer_t& renderer) const;
private:
	bool_t visible { false };
	draw_scheme_t scheme {};
	draw_count_t count {};
};

struct inventory_gui_t : public not_copyable_t {
public:
	inventory_gui_t() = default;
	inventory_gui_t(inventory_gui_t&&) noexcept = default;
	inventory_gui_t& operator=(inventory_gui_t&&) noexcept = default;
	~inventory_gui_t() = default;
public:
	bool init();
	void handle(const input_t& input, audio_t& audio, kernel_t& kernel, receiver_t& receiver, const stack_gui_t& stack_gui, const dialogue_gui_t& dialogue_gui, headsup_gui_t& headsup_gui);
	void update(real64_t delta);
	void render(renderer_t& renderer, const kernel_t& kernel) const;
	bool open() const;
private:
	mutable bool_t amend { true };
	mutable arch_t current { 0 };
	bool_t ready { false };
	std::vector<inventory_gui_element_t> elements {};
};
