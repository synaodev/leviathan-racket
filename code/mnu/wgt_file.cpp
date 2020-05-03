#include "./wgt_file.hpp"

#include "../utl/vfs.hpp"
#include "../utl/setup_file.hpp"
#include "../sym/input.hpp"
#include "../sym/video.hpp"
#include "../sym/audio.hpp"
#include "../sym/kernel.hpp"
#include "../oly/draw_headsup.hpp"

#include "../res/id.hpp"

static constexpr arch_t kTotalOptions = 2;
static const glm::vec2 kDefaultPosition = glm::vec2(4.0f, 2.0f);

wgt_file_t::wgt_file_t(arch_t flags) :
	widget_i(flags),
	file_op(wgt_file_op_t::Unknown),
	cursor(0),
	text(),
	arrow()
{

}

void wgt_file_t::init(const input_t&, const video_t&, audio_t&, const music_t&, kernel_t& kernel) {
	ready = true;
	cursor = kernel.get_file_index();
	kernel.freeze();
	text.set_font(vfs::font(0));
	text.set_position(kDefaultPosition);
	if (bitmask[0]) {
		text.set_string(vfs::i18n_find("FileSys", 0));
		text.append_string(vfs::i18n_find("FileSys", 2, 4));
	} else {
		text.set_string(vfs::i18n_find("FileSys", 1, 4));
	}
	arrow.set_file(vfs::animation(res::anim::Heads));
	arrow.set_state(1);
	arrow.set_position(
		text.get_font_size().x, 
		4.0f + kDefaultPosition.y + 
		(text.get_font_size().y * 2.0f)
	);
	arrow.mut_position(
		0.0f, static_cast<real_t>(cursor) * 
		text.get_font_size().y
	);
}

void wgt_file_t::handle(setup_file_t&, input_t& input, video_t&, audio_t& audio, music_t&, kernel_t& kernel, stack_gui_t&, draw_headsup_t& headsup) {
	if (input.pressed[btn_t::Up]) {
		if (cursor > 0) {
			--cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, -text.get_font_size().y);
		}
	} else if (input.pressed[btn_t::Down]) {
		if (cursor < kTotalOptions) {
			++cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, text.get_font_size().y);
		}
	} else if (input.pressed[btn_t::Yes]) {
		active = false;
		file_op = bitmask.any() ? wgt_file_op_t::Saving : wgt_file_op_t::Loading;
		audio.play(res::sfx::TitleBeg, 0);
		kernel.set_file_index(cursor);
	} else if (input.pressed[btn_t::No] or input.pressed[btn_t::Options]) {
		active = false;
		file_op = wgt_file_op_t::Unknown;
	}
	if (!active) {
		input.pressed.reset();
		switch (file_op) {
		case wgt_file_op_t::Unknown:
			kernel.unlock();
			break;
		case wgt_file_op_t::Loading:
			headsup.fade_out();
			kernel.load_progress();
			break;
		case wgt_file_op_t::Saving:
			kernel.save_progress();
			kernel.unlock();
			break;
		default:
			break;
		}
	}
}

void wgt_file_t::update(real64_t delta) {
	if (ready and active) {
		arrow.update(delta);
	}
}

void wgt_file_t::render(renderer_t& renderer) const {
	if (ready and active) {
		text.render(renderer);
		arrow.render(renderer);
	}
}

void wgt_file_t::force() const {
	if (ready and active) {
		text.force();
		arrow.force();
	}
}