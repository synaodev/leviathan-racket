#ifndef SYNAO_MENU_WIDGET_FILE_HPP
#define SYNAO_MENU_WIDGET_FILE_HPP

#include "./widget.hpp"

#include "../overlay/draw_text.hpp"
#include "../overlay/draw_scheme.hpp"

namespace __enum_wgt_file_op {
	enum type : arch_t {
		Unknown,
		Loading,
		Saving
	};
}

using wgt_file_op_t = __enum_wgt_file_op::type;

struct wgt_file_t : public widget_i {
public:
	wgt_file_t(arch_t flags);
	wgt_file_t(wgt_file_t&&) = default;
	wgt_file_t& operator=(wgt_file_t&&) = default;
	~wgt_file_t() = default;
public:
	void init(const input_t& input, const video_t& video, audio_t& audio, const music_t& music, kernel_t& kernel) override;
	void handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, draw_headsup_t& headsup) override;
	void update(real64_t delta) override;
	void render(renderer_t& renderer) const override;
	void force() const override;
private:
	wgt_file_op_t file_op;
	arch_t cursor;
	draw_text_t text;
	draw_scheme_t arrow;
};

#endif // SYNAO_MENU_WIDGET_FILE_HPP