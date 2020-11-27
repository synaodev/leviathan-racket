#include "./wgt-option.hpp"
#include "./stack-gui.hpp"
#include "./headsup-gui.hpp"

#include "../resource/id.hpp"
#include "../resource/vfs.hpp"
#include "../system/input.hpp"
#include "../system/audio.hpp"
#include "../system/kernel.hpp"

static constexpr arch_t kOptionTotalOptions = 6;
static const glm::vec2 kOptionDefaultPosition = glm::vec2(4.0f, 2.0f);

wgt_option_t::wgt_option_t(arch_t flags) :
	widget_i(flags),
	cursor(0),
	text(),
	arrow()
{

}

void wgt_option_t::init(const input_t&, const video_t&, audio_t& audio, const music_t&, kernel_t& kernel) {
	ready = true;
	audio.play(res::sfx::Inven);
	kernel.freeze();
	text.set_font(vfs::font(0));
	text.set_position(kOptionDefaultPosition);
	text.set_string(vfs::i18n_find("Options", 0, 7));
	arrow.set_file(vfs::animation(res::anim::Heads));
	arrow.set_state(1);
	arrow.set_position(
		text.get_font_size().x,
		4.0f + kOptionDefaultPosition.y +
		(text.get_font_size().y * 2.0f)
	);
}

void wgt_option_t::handle(setup_file_t&, input_t& input, video_t&, audio_t& audio, music_t&, kernel_t& kernel, stack_gui_t& stack_gui, headsup_gui_t& headsup_gui) {
	bool reboot = false;
	if (input.pressed[btn_t::Up]) {
		if (cursor > 0) {
			--cursor;
			audio.play(res::sfx::Select);
			arrow.mut_position(0.0f, -text.get_font_size().y);
		}
	} else if (input.pressed[btn_t::Down]) {
		if (cursor < kOptionTotalOptions) {
			++cursor;
			audio.play(res::sfx::Select);
			arrow.mut_position(0.0f, text.get_font_size().y);
		}
	} else if (input.pressed[btn_t::Yes]) {
		switch (cursor) {
		case 0: // Return
			active = false;
			break;
		case 1: // Input
			audio.play(res::sfx::Inven);
			stack_gui.push(menu_t::Input, 0);
			break;
		case 2: // Video
			audio.play(res::sfx::Inven);
			stack_gui.push(menu_t::Video, 0);
			break;
		case 3: // Audio
			audio.play(res::sfx::Inven);
			stack_gui.push(menu_t::Audio, 0);
			break;
		case 4: // Language
			audio.play(res::sfx::Inven);
			stack_gui.push(menu_t::Language, 0);
			break;
		case 5: // Reboot
			reboot = true;
			active = false;
			break;
		case 6: // Exit
			kernel.quit();
			break;
		default:
			break;
		}
	} else if (input.pressed[btn_t::No] or input.pressed[btn_t::Options]) {
		active = false;
	}
	if (!active) {
		input.flush();
		if (reboot) {
			audio.play(res::sfx::TitleBeg, 0);
			headsup_gui.fade_out();
			stack_gui.clear();
			kernel.boot();
		} else {
			audio.play(res::sfx::Inven, 0);
			kernel.unlock();
		}
	}
}

void wgt_option_t::update(real64_t delta) {
	if (ready and active) {
		arrow.update(delta);
	}
}

void wgt_option_t::render(renderer_t& renderer) const {
	if (ready and active) {
		text.render(renderer);
		arrow.render(renderer);
	}
}

void wgt_option_t::invalidate() const {
	if (ready and active) {
		text.invalidate();
		arrow.invalidate();
	}
}
