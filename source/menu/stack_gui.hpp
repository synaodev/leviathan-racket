#ifndef LEVIATHAN_INCLUDED_MENU_STACK_GUI_HPP
#define LEVIATHAN_INCLUDED_MENU_STACK_GUI_HPP

#include <vector>
#include <memory>

#include "./widget.hpp"

struct draw_title_view_t;
struct draw_headsup_t;

struct stack_gui_t : public not_copyable_t {
public:
	stack_gui_t();
	stack_gui_t(stack_gui_t&&) = default;
	stack_gui_t& operator=(stack_gui_t&&) = default;
	~stack_gui_t() = default;
public:
	void reset();
	void handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, draw_title_view_t& title_view, draw_headsup_t& headsup);
	void update(real64_t delta);
	void render(renderer_t& renderer, const inventory_gui_t& inventory_gui) const;
	void invalidate() const;
	void push(menu_t type, arch_t flags);
	void pop();
	void clear();
	bool empty() const;
private:
	mutable bool_t amend;
	bool_t release;
	std::vector<std::unique_ptr<widget_i> > widgets;
};

#endif // LEVIATHAN_INCLUDED_MENU_STACK_GUI_HPP
