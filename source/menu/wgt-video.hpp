#ifndef LEVIATHAN_INCLUDED_MENU_WIDGET_VIDEO_HPP
#define LEVIATHAN_INCLUDED_MENU_WIDGET_VIDEO_HPP

#include "./widget.hpp"

#include "../overlay/draw-text.hpp"
#include "../overlay/draw-scheme.hpp"

struct screen_params_t;

struct wgt_video_t : public widget_i {
public:
	wgt_video_t() = default;
	wgt_video_t(arch_t flags) :
		widget_i(flags) {}
	wgt_video_t(wgt_video_t&&) = default;
	wgt_video_t& operator=(wgt_video_t&&) = default;
	~wgt_video_t() = default;
public:
	void init(const input_t& input, const video_t& video, audio_t& audio, const music_t& music, kernel_t& kernel) override;
	void handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, headsup_gui_t& headsup_gui) override;
	void update(real64_t delta) override;
	void render(renderer_t& renderer) const override;
	void invalidate() const override;
private:
	void setup_text(const screen_params_t& params);
private:
	arch_t cursor { 0 };
	draw_text_t text {};
	draw_scheme_t arrow {};
};

#endif // LEVIATHAN_INCLUDED_MENU_WIDGET_VIDEO_HPP
