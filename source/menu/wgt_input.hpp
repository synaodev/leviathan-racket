#ifndef LEVIATHAN_INCLUDED_MENU_WIDGET_INPUT_HPP
#define LEVIATHAN_INCLUDED_MENU_WIDGET_INPUT_HPP

#include "./widget.hpp"

#include "../overlay/draw_text.hpp"
#include "../overlay/draw_scheme.hpp"

struct wgt_input_t : public widget_i {
public:
	wgt_input_t(arch_t flags);
	wgt_input_t(wgt_input_t&&) = default;
	wgt_input_t& operator=(wgt_input_t&&) = default;
	~wgt_input_t() = default;
public:
	void init(const input_t& input, const video_t& video, audio_t& audio, const music_t& music, kernel_t& kernel) override;
	void handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, headsup_gui_t& headsup_gui) override;
	void update(real64_t delta) override;
	void render(renderer_t& renderer) const override;
	void invalidate() const override;
private:
	void setup_text(const input_t& input);
private:
	bool_t siding;
	bool_t waiting;
	bool_t flashed;
	arch_t cursor;
	draw_text_t header, left_text, right_text;
	draw_scheme_t arrow;
};

#endif // LEVIATHAN_INCLUDED_MENU_WIDGET_INPUT_HPP
