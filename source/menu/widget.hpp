#pragma once

#include <bitset>

#include "../types.hpp"

enum class menu_t : arch_t {
	Option,
	File,
	Input,
	Video,
	Audio,
	Language
};

struct config_t;

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
	widget_i() = default;
	widget_i(arch_t flags) :
		bitmask(flags) {}
	widget_i(widget_i&&) = default;
	widget_i& operator=(widget_i&&) = default;
	virtual ~widget_i() = default;
public:
	virtual void init(const input_t& input, const video_t& video, audio_t& audio, const music_t& music, kernel_t& kernel) = 0;
	virtual void handle(config_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, headsup_gui_t& headsup_gui) = 0;
	virtual void update(real64_t delta) = 0;
	virtual void render(renderer_t& renderer) const = 0;
	virtual void invalidate() const = 0;
	inline bool is_ready() const { return ready; }
	inline bool is_active() const { return active; }
	bool get_flag(arch_t index) const {
		if (index >= bitmask.size()) {
			return false;
		}
		return bitmask[index];
	}
protected:
	bool_t ready { false };
	bool_t active { true };
	std::bitset<8> bitmask { 0 };
};
