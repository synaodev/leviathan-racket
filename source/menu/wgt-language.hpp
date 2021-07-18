#pragma once

#include "./widget.hpp"

#include "../overlay/draw-text.hpp"
#include "../overlay/draw-scheme.hpp"

struct wgt_language_t : public widget_i {
public:
	wgt_language_t() = default;
	wgt_language_t(arch_t flags) :
		widget_i(flags) {}
	wgt_language_t(wgt_language_t&&) = default;
	wgt_language_t& operator=(wgt_language_t&&) = default;
	~wgt_language_t() = default;
public:
	void init(const input_t& input, const video_t& video, audio_t& audio, const music_t& music, kernel_t& kernel) override;
	void handle(config_t&, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, headsup_gui_t& headsup_gui) override;
	void update(real64_t delta) override;
	void render(renderer_t& renderer) const override;
	void invalidate() const override;
private:
	void setup_text();
private:
	std::vector<std::string> languages {};
	arch_t cursor { 0 };
	arch_t first { 0 };
	arch_t last { 0 };
	draw_text_t text {};
	draw_scheme_t arrow {};
};
