#include "./wgt-file.hpp"
#include "./headsup-gui.hpp"

#include "../resource/id.hpp"
#include "../resource/vfs.hpp"
#include "../system/input.hpp"
#include "../system/video.hpp"
#include "../system/audio.hpp"
#include "../system/kernel.hpp"
#include "../utility/setup-file.hpp"

namespace {
	constexpr arch_t kFileTotalOptions 		= 2;
	const glm::vec2 kFileDefaultPosition 	{ 4.0f, 2.0f };
}

void wgt_file_t::init(const input_t&, const video_t&, audio_t&, const music_t&, kernel_t& kernel) {
	ready = true;
	cursor = kernel.get_file_index();
	kernel.freeze();
	text.set_font(vfs_t::font(0));
	text.set_position(kFileDefaultPosition);
	if (bitmask[0]) {
		text.set_string(vfs_t::i18n_find("FileSys", 0));
		text.append_string(vfs_t::i18n_find("FileSys", 2, 4));
	} else {
		text.set_string(vfs_t::i18n_find("FileSys", 1, 4));
	}
	arrow.set_file(vfs_t::animation(res::anim::Heads));
	arrow.set_state(1);
	arrow.set_position(
		text.get_font_size().x,
		4.0f + kFileDefaultPosition.y +
		(text.get_font_size().y * 2.0f)
	);
	arrow.mut_position(
		0.0f, static_cast<real_t>(cursor) *
		text.get_font_size().y
	);
}

void wgt_file_t::handle(setup_file_t&, input_t& input, video_t&, audio_t& audio, music_t&, kernel_t& kernel, stack_gui_t&, headsup_gui_t& headsup_gui) {
	if (input.pressed[btn_t::Up]) {
		if (cursor > 0) {
			--cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, -text.get_font_size().y);
		}
	} else if (input.pressed[btn_t::Down]) {
		if (cursor < kFileTotalOptions) {
			++cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, text.get_font_size().y);
		}
	} else if (input.pressed[btn_t::Yes]) {
		active = false;
		file_op = bitmask.any() ?
			file_op_t::Saving :
			file_op_t::Loading;
		audio.play(res::sfx::TitleBeg, 0);
		kernel.set_file_index(cursor);
	} else if (input.pressed[btn_t::No] or input.pressed[btn_t::Options]) {
		active = false;
		file_op = file_op_t::Unknown;
	}
	if (!active) {
		input.flush();
		switch (file_op) {
		case file_op_t::Unknown:
			kernel.unlock();
			break;
		case file_op_t::Loading:
			headsup_gui.fade_out();
			kernel.load_progress();
			break;
		case file_op_t::Saving:
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

void wgt_file_t::invalidate() const {
	if (ready and active) {
		text.invalidate();
		arrow.invalidate();
	}
}
