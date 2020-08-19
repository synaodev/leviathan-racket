#ifndef LEVIATHAN_INCLUDED_MENU_WIDGET_FIELD_HPP
#define LEVIATHAN_INCLUDED_MENU_WIDGET_FIELD_HPP

#include "./widget.hpp"

#include "../overlay/draw_text.hpp"
#include "../overlay/draw_scheme.hpp"

struct wgt_field_t : public widget_i {
public:
	wgt_field_t(arch_t flags);
	wgt_field_t(wgt_field_t&&) = default;
	wgt_field_t& operator=(wgt_field_t&&) = default;
	~wgt_field_t() = default;
public:
	void init(const input_t& input, const video_t& video, audio_t& audio, const music_t& music, kernel_t& kernel) override;
	void handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, draw_headsup_t& headsup) override;
	void update(real64_t delta) override;
	void render(renderer_t& renderer) const override;
	void invalidate() const override;
private:
	void setup_listing();
	void setup_identity();
private:
	std::vector<std::string> fields;
	bool_t scrolling;
	sint_t identity;
	arch_t cursor, first, last;
	draw_text_t text_listing, text_identity;
	draw_scheme_t arrow;
};

#endif // LEVIATHAN_INCLUDED_MENU_WIDGET_FIELD_HPP
