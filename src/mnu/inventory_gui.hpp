#ifndef SYNAO_MENU_INVENTORY_GUI_HPP
#define SYNAO_MENU_INVENTORY_GUI_HPP

#include "../oly/draw_count.hpp"
#include "../oly/draw_scheme.hpp"

struct input_t;
struct audio_t;
struct kernel_t;
struct receiver_t;
struct stack_gui_t;
struct dialogue_gui_t;
struct draw_title_view_t;

struct inventory_gui_element_t : public not_copyable_t {
public:
	inventory_gui_element_t();
	inventory_gui_element_t(inventory_gui_element_t&&) = default;
	inventory_gui_element_t& operator=(inventory_gui_element_t&&) = default;
	~inventory_gui_element_t() = default;
public:
	void init(const texture_t* texture, const palette_t* palette, const animation_t* animation, arch_t index);
	void reset(glm::ivec4 item);
	void update(real64_t delta);
	void render(renderer_t& renderer) const;
private:
	bool_t visible;
	draw_scheme_t scheme;
	draw_count_t count;
};

struct inventory_gui_t : public not_copyable_t {
public:
	inventory_gui_t();
	inventory_gui_t(inventory_gui_t&&) = default;
	inventory_gui_t& operator=(inventory_gui_t&&) = default;
	~inventory_gui_t() = default;
public:
	bool init();
	void handle(const input_t& input, audio_t& audio, kernel_t& kernel, receiver_t& receiver, const stack_gui_t& stack_gui, const dialogue_gui_t& dialogue_gui, draw_title_view_t& title_view);
	void update(real64_t delta);
	void render(renderer_t& renderer, const kernel_t& kernel) const;
	bool open() const;
private:
	mutable bool_t write;
	mutable arch_t current;
	bool_t ready;
	std::vector<inventory_gui_element_t> elements;
};

#endif // SYNAO_MENU_INVENTORY_GUI_HPP