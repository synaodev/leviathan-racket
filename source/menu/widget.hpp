#ifndef LEVIATHAN_INCLUDED_MENU_WIDGET_HPP
#define LEVIATHAN_INCLUDED_MENU_WIDGET_HPP

#include <bitset>

#include "../types.hpp"

namespace __enum_menu {
	enum type : arch_t {
		Option,
		File,
		Input,
		Video,
		Audio,
		Language
	};
}

using menu_t = __enum_menu::type;

struct setup_file_t;

struct input_t;
struct video_t;
struct audio_t;
struct music_t;
struct renderer_t;
struct kernel_t;
struct stack_gui_t;
struct inventory_gui_t;
struct headsup_gui_t;

struct widget_i : public not_copyable_t {
public:
	widget_i();
	widget_i(arch_t flags);
	widget_i(widget_i&&) = default;
	widget_i& operator=(widget_i&&) = default;
	virtual ~widget_i() = default;
public:
	virtual void init(const input_t& input, const video_t& video, audio_t& audio, const music_t& music, kernel_t& kernel) = 0;
	virtual void handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, headsup_gui_t& headsup_gui) = 0;
	virtual void update(real64_t delta) = 0;
	virtual void render(renderer_t& renderer) const = 0;
	virtual void invalidate() const = 0;
	bool is_ready() const;
	bool is_active() const;
	bool get_flag(arch_t index) const;
protected:
	bool_t ready, active;
	std::bitset<8> bitmask;
};

#endif // LEVIATHAN_INCLUDED_MENU_WIDGET_HPP
