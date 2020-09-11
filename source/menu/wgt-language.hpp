#ifndef LEVIATHAN_INCLUDED_MENU_WIDGET_LANGUAGE_HPP
#define LEVIATHAN_INCLUDED_MENU_WIDGET_LANGUAGE_HPP

#include "./widget.hpp"

#include "../overlay/draw-text.hpp"
#include "../overlay/draw-scheme.hpp"

struct wgt_language_t : public widget_i {
public:
	wgt_language_t(arch_t flags);
	wgt_language_t(wgt_language_t&&) = default;
	wgt_language_t& operator=(wgt_language_t&&) = default;
	~wgt_language_t() = default;
public:
	void init(const input_t& input, const video_t& video, audio_t& audio, const music_t& music, kernel_t& kernel) override;
	void handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, headsup_gui_t& headsup_gui) override;
	void update(real64_t delta) override;
	void render(renderer_t& renderer) const override;
	void invalidate() const override;
private:
	void setup_text();
private:
	std::vector<std::string> languages;
	arch_t cursor, first, last;
	draw_text_t text;
	draw_scheme_t arrow;
};

#endif // LEVIATHAN_INCLUDED_MENU_WIDGET_LANGUAGE_HPP
