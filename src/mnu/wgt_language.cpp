#include "./wgt_language.hpp"
#include "./stack_gui.hpp"

#include "../utl/vfs.hpp"
#include "../utl/setup_file.hpp"

#include "../sys/input.hpp"
#include "../sys/audio.hpp"
#include "../sys/kernel.hpp"

#include "../oly/draw_headsup.hpp"

#include "../res_id.hpp"

static constexpr arch_t kTotalVisible = 9;
static const glm::vec2 kDefaultPosition = glm::vec2(4.0f, 2.0f);
static const glm::vec2 kBottomPosition = glm::vec2(120.0f, 2.0f);

wgt_language_t::wgt_language_t(arch_t flags) :
	widget_i(flags),
	languages(),
	cursor(0),
	first(0),
	last(0),
	text(),
	arrow()
{

}

void wgt_language_t::init(video_t& video, audio_t& audio, music_t& music, kernel_t& kernel) {
	ready = true;
	languages = vfs::file_list("./data/event/i18n/");
	text.set_font(vfs::font(0));
	text.set_position(kDefaultPosition);
	arrow.set_file(vfs::animation(res::anim::Heads));
	arrow.set_state(4);
	arrow.set_position(
		text.get_font_size().x, 
		4.0f + kDefaultPosition.y + 
		(text.get_font_size().y * 2.0f)
	);
	this->setup_text();
}

void wgt_language_t::handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, draw_headsup_t& headsup) {
	bool selection = false;
	if (input.pressed[btn_t::Up]) {
		if (cursor > 0) {
			--cursor;
			if (cursor < first) {
				--first;
				--last;
				this->setup_text();
			} else {
				arrow.mut_position(
					0.0f, -text.get_font_size().y
				);
			}
			audio.play(res::sfx::Select, 0);
		}
	} else if (input.pressed[btn_t::Down]) {
		if (cursor < languages.size() - 1) {
			++cursor;
			if (cursor >= last) {
				++first;
				++last;
				this->setup_text();
			} else {
				arrow.mut_position(
					0.0f, text.get_font_size().y
				);
			}
			audio.play(res::sfx::Select, 0);
		}
	} else if (input.pressed[btn_t::Yes]) {
		active = false;
		selection = true;
	}
	if (input.pressed[btn_t::No] or input.pressed[btn_t::Options]) {
		active = false;
	}
	if (!active) {
		input.pressed.reset();
		if (!selection) {
			audio.play(res::sfx::Inven, 0);
		} else if (vfs::try_language(languages[cursor])) {
			config.set("Setup", "Language", languages[cursor]);
			audio.play(res::sfx::TitleBeg, 0);
			headsup.fade_out();
			stack_gui.clear();
			kernel.boot();
		}
	}
}

void wgt_language_t::update(real64_t delta) {
	if (ready and active) {
		arrow.update(delta);
	}
}

void wgt_language_t::render(renderer_t& renderer) const {
	if (ready and active) {
		text.render(renderer);
		arrow.render(renderer);
	}
}

void wgt_language_t::force() const {
	if (ready and active) {
		text.force();
		arrow.force();
	}
}

void wgt_language_t::setup_text() {
	text.set_string(vfs::i18n_find("Language", 0));
	for (arch_t it = first; it < languages.size() and it != last; ++it) {
		text.append_string("\t " + languages[it] + '\n');
	}
}